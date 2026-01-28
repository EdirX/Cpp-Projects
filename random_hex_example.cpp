/*
COMMENTS:
 */

// -------------------------------
// -------------------------------
// INCLUDES
#include<iostream>
#include <sstream>
#include<vector>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <random>

using namespace std;

// --------------------------------------------------
// OVERLOADING
// --------------------------------------------------

// Print pair
template <class T>
ostream &operator<<(std::ostream &out, const pair<T, T> &p)
{
    out << "(" << p.first << ", " << p.second << ")";
    return out;
}

// Print unordered map
template <class T>
ostream &operator<<(std::ostream &out, const unordered_map<T, vector<T>> &p)
{
    for(auto el:p)
    {
        out << el.first << " :" << endl;
        for(auto vec_el:el.second)
            out << vec_el << " ";
        
        out << endl;
    }
    return out;
}

// -------------------------------
// -------------------------------
// ENUMS
// ------------
// Player Colour
enum class PlayerColour : int
{
    unvisited = 0,
    red,
    blue
};

//-- output --
ostream &operator<<(ostream &os, const PlayerColour &p)
{
    switch (p)
    {
    case (PlayerColour::unvisited):
        os << " . ";
        break;
    case (PlayerColour::red):
        os << " R ";
        break;
    case (PlayerColour::blue):
        os << " B ";
        break;
    default:
        os << " ? ";
    }
    return os;
}
// ------------
ostream &operator<<(ostream &os, const vector<PlayerColour> &row)
{
    for (int j = 0; j < row.size() - 1; j++)
    {
        os << row[j] << "-";
    }
    //last entry - no "-"
    os << row[row.size() - 1] << endl;

    return os;
}

// -------------------------------
// -------------------------------
// CLASSES
// ----------
// Hex

class Hex
{
    public:
        // -- Constructors --
        Hex(int board_size=7):board_size(board_size){
            // blue moves first
            blue_moves = true;

            // Construct the board
            construct();
        }

        // Reset the game
        void reset()
        {
            // Human Player
            string human_colour;
            bool invalid_colour = true;


            // Clear console
            // clearScreen();

            cout << "\nWelcome to Hex.\nThe board size is " << board_size << "x" << board_size << ".\nBlue Starts the game.\n";
            while (invalid_colour)
            {
                cout << "Please enter human Player Colour(Red|Blue)?";
                cin >> human_colour;
                invalid_colour = testColour(human_colour);
            }

            // initialise valid moves
            valid_moves_left = true;

            // blue moves first
            blue_moves = true;

            // reset winner
            winner = PlayerColour::unvisited;

            // Clear blue graph components
            blue_nodes.clear();
            red_nodes.clear();

            //delete the existing vectors
            for (auto k : blue_neighbours)
                vector<int>().swap(k.second); //delete the existing vectors
            for (auto k : red_neighbours)
                vector<int>().swap(k.second); //delete the existing vectors

            // clear the maps
            blue_neighbours.clear();
            red_neighbours.clear();
        }

        //Run the game
        void run()
        {
            //Print board
            cout << toString();
            // RUn until win or no more valid moves
            while ((winner == PlayerColour::unvisited) && valid_moves_left)
            {
                // Clear for next move
                // clearScreen();
                // cout << toString();

                // check if space left
                if (!spaceLeft())
                {
                    cout << "\nNo more space left for play.\n";
                    break;
                }

                pair<int, int> coords;

                if(blue_moves && human == PlayerColour::blue)
                {
                    coords = inputMove();
                    cout << toString();
                }
                else if (blue_moves && computer == PlayerColour::blue)
                {
                    coords = computerMove();
                    cout << "COMPUTER MOVES:(" << coords.first << ", " << coords.second << ")\n";
                    board[coords.first][coords.second] = PlayerColour::blue;
                    cout << toString();
                }
                else if (!blue_moves && human == PlayerColour::red)
                {
                    coords = inputMove();
                    cout << toString();
                }
                else
                {
                    coords = computerMove();
                    cout << "COMPUTER:(" << coords.first << ", " << coords.second << ")\n";
                    board[coords.first][coords.second] = PlayerColour::red;
                    cout << toString();
                }

                vector<pair<int, int>> neighbours = getNeighbours(coords);

                // // Computer moves
                // pair<int,int> computerCoords = computerMove();
                // cout << "COMPUTER:(" << computerCoords.first << ", " << computerCoords.second << ")\n";

                // And add neighbours
                vector<int> converted_neighbours;
                for (auto n : neighbours)
                    converted_neighbours.push_back(convertCoordToInt(n));

                // cout << "Converted Neighbours: ";
                // for (auto x : converted_neighbours)
                //     cout << x << " ";
                // cout << endl;


                // Add blue and red graph info
                if(blue_moves)
                    updateNodesNeighbours(coords, converted_neighbours, blue_nodes, blue_neighbours);
                else
                    updateNodesNeighbours(coords, converted_neighbours, red_nodes, red_neighbours);

                if (blue_moves)
                {
                    if (checkWin(blue_moves, blue_nodes, blue_neighbours))
                    {
                        if(human == PlayerColour::blue)
                            cout << "The human wins!\n";
                        else
                        {
                            cout << "The computer wins!\n";
                        }

                        break;
                    }
                }
                else
                {
                    if (checkWin(blue_moves, red_nodes, red_neighbours))
                    {
                        if (human == PlayerColour::red)
                            cout << "The human wins!\n";
                        else
                        {
                            cout << "The computer wins!\n";
                        }

                        break;
                    }
                }

                // next player moves
                blue_moves = !blue_moves;
            }
        }

        // Check for a win
        bool checkWin(bool blue_moves, vector<int> &node_vec, unordered_map<int, vector<int>> &neighbours)
        {
            //check if completed
            vector<int> first_side, opposing_side;

            if (blue_moves)
            {
                for (auto n : node_vec)
                {
                    if (n % board_size == 0)
                        first_side.push_back(n);
                    else if (n % board_size == board_size - 1)
                        opposing_side.push_back(n);
                }
            }
            else
            {
                for (auto n : node_vec)
                {
                    if (n < board_size )
                        first_side.push_back(n);
                    else if (n  >= board_size * (board_size - 1))
                        opposing_side.push_back(n);
                }
            }
            
            // cout << "SIDE 1:";
            // for (auto n : first_side)
            //     cout << n << " ";
            // cout << endl;

            // cout << "SIDE 2:";
            // for (auto n : opposing_side)
            //     cout << n << " ";
            // cout << endl;


            //Only check if both sides have elements
            if ((first_side.size() > 0) && (opposing_side.size() > 0))
            {
                for(auto start:first_side)
                    for (auto end : opposing_side)
                    {
                        pair<int, vector<int>> path_length = dijkstraPathLength(true, start, end, node_vec, neighbours);
                        // cout << "Start: " << start << " End: " << end << " | Dijkstra Path Length:" << path_length.first << endl;

                        if (path_length.first > 0)
                            return true;
                    }
            }

            return false;
        }

        // path length using dijkstra
        pair<int,vector<int>> dijkstraPathLength(bool shortest, int start, int end, const vector<int> &node_vec, unordered_map<int, vector<int>> &neighbours)
        {
            // cout << "START: " << start << " END: " << end << endl;

            int used_int = -1;
            int path_length = -1;

            if (shortest)
                used_int = MAX_INT;

            vector<int> visited;
            unordered_map<int, int> unvisited;
            int current_node = start;

            // Set all nodes to infinity
            // except current node
            for(auto i: node_vec)
                unvisited[i] = used_int;
            unvisited[current_node] = 0;

            // cout << "UNVISITED";
            // for(auto i:unvisited)
            //     cout << "(" << i.first << "," << i.second << ")";
            // cout << endl;

            // bool exit_on_tentative_dist = true;

            if (shortest)
            {
                int new_node = 0;

                while ((find(visited.begin(), visited.end(), end) == visited.end()) && (new_node != -1))
                {

                    if(current_node == end)
                    {
                        visited.push_back(end);

                        // cout << "SUCCESS: " << visited.size() - 1 << endl;
                        // cout << "EXIT 1 - SUCCESS\n";

                        return pair<int, vector<int>>(visited.size() - 1, visited);
                    }

                    // update neighbour values
                    vector<int> &current_neighbours = neighbours[current_node];

                    // cout << "CURRENT " << current_node << " NEIGHBOURS:";
                    // for (auto a : current_neighbours)
                    //     cout << a << " ";
                    // cout << endl;

                    for (auto neighbour : current_neighbours)
                    {
                        // don't do for visited
                        if ( find(visited.begin(), visited.end(), neighbour) == visited.end())
                        {

                            int update = unvisited[current_node] + 1;
                            if (update < unvisited[neighbour])
                            {
                                unvisited[neighbour] = update;
                            }
                        }
                    }

                    // remove from unvisited
                    path_length = unvisited[current_node];
                    visited.push_back(current_node);
                    unvisited.erase(current_node);

                    //sort
                    int node_val = used_int;
                    new_node=-1;

                    for (auto it : unvisited)
                    {
                        // cout << "(" << it.first << "," << it.second << ")";
                        if (it.second < node_val)
                        {
                            node_val = it.second;
                            new_node = it.first;
                        }
                    }

                    // cout << "\nNEWNODE:" << new_node << endl;

                    if (new_node == -1)
                    {
                        // cout << "EXIT 2 - FAIL\n";
                        return pair<int, vector<int>>(-1, visited);
                    }
                
                    // cout << "VISITED:";
                    // for(auto a: visited)
                    //     cout << a <<" ";
                    // cout << endl;
                    // if (visited.size() > 10)
                    // {
                    //     cout << " FFS - SIZE ERROR\n";
                    //     throw("aa");
                    // }
                    current_node = new_node;
                }
            }

            // cout << "EXIT 3 - FAIL\n";
            return pair<int, vector<int>>(-1, visited);
        }

        // Input move
        pair<int, int> inputMove()
        {
            bool not_valid_input = true;
            pair<int, int> output;

            while(not_valid_input)
            {
                string coords_str; // coords as string
                vector<int> coords; // coords as int
                cout << "Enter coordinate as row,column (starts at (0,0)): ";
                cin >> coords_str;

                stringstream s_stream(coords_str);

                //convert
                while (s_stream.good())
                {
                    string substr;
                    getline(s_stream, substr, ','); //get first string delimited by comma
                    coords.push_back(stoi(substr));
                }
                // Checks
                // Check if 2 inputs
                
                if (coords.size()!=2)
                {
                    cout << "Please enter 2 coordinates in the form row, column\n";
                }else
                {
                    // Check if on board
                    if ((coords[0] < 0) || (coords[0] >= board.size()) || (coords[1] < 0) || (coords[1] >= board.size()))
                        cout << "Coordinates off the board. Range is (0,0) to (" << board.size() - 1 << ", " << board.size() - 1 <<").\n";
                    else
                    {

                        // cout << "Current:" << board[coords[0]][coords[1]] << endl;

                        // Check if already taken
                        if (board[coords[0]][coords[1]] == PlayerColour::unvisited) //ok to override
                        {
                            if (blue_moves)
                                board[coords[0]][coords[1]] = PlayerColour::blue;
                            else
                                board[coords[0]][coords[1]] = PlayerColour::red;

                            output.first = coords[0];
                            output.second = coords[1];
                           
                            return output;

                            not_valid_input = false; //input ok, let's move on
                        }
                        else
                        {
                            cout << "Invalid move - coordinate (" << coords[0] << "," << coords[1] << ") is already taken by" << board[coords[0]][coords[1]] << ".\n";
                        }
                    }
                }
            }
            return output;
        }
        

        // -- Destructor --
        ~Hex(){
            // destruct each board row
            for(auto row:board)
            {
                // cout << "The row vector size is " << row.size() << ", and its "
                //      << "capacity is " << row.capacity() << endl;
                vector<PlayerColour>().swap(row);
                // cout << "The row vector size is " << row.size() << ", and its "
                //      << "capacity is " << row.capacity() << endl;
            }

            // destruct the board
            vector<vector<PlayerColour>>().swap(board);
        }

        // -- Member functions --
        string toString()
        {
            stringstream os;

            // Who's move is it
            if (blue_moves)
            {
                os << "\n-----------\n";
                os << "Blue (" << PlayerColour::blue << ") Moves (West to East).";
                os << "\n-----------\n\n";
            }
            else
            {
                os << "\n-----------\n";
                os << "Red (" << PlayerColour::red << ") Moves (North to South).";
                os << "\n-----------\n\n";
            }

            // The board
            os << printBoard(board);

            return os.str();
        }

        // ostream output
        friend ostream &operator<<(ostream &os, Hex &h)
        {

            os << h.toString();
            return os;
        }


    protected:
        int board_size;
        PlayerColour human, computer, winner;
        vector<vector<PlayerColour>> board; // board is vector of vectors
        bool valid_moves_left; // valid moves left?
        bool blue_moves; // is blue the current player

        unordered_map<int, vector<int>> blue_neighbours; // the reachable neighbours from a node
        unordered_map<int, vector<int>> red_neighbours; // the reachable neighbours from a node
        vector<int> blue_nodes;
        vector<int> red_nodes;

        const int MAX_INT = numeric_limits<int>::max();

        // Convert coord to integer
        int
        convertCoordToInt(pair<int, int> coord)
        {
            return coord.first * board_size + coord.second;
        }

        // Hack to clear screen
        void clearScreen()
        {
            cout << string(100, '\n');
        }

        // Construct the board
        void construct()
        {
            // setup the board
            for (int i=0; i < board_size; ++i)
            {
                vector<PlayerColour> board_row(board_size);
                board.push_back(board_row);
            }
        }

        // Test if valid colour input
        bool testColour(string input_colour)
        {
            if (input_colour == "Red")
            {
                human = PlayerColour::red;
                computer = PlayerColour::blue;
                cout << "OK! Human is Red(" << human << "). Computer is Blue(" << computer << ")\n";
                return false;
            }

            if (input_colour == "Blue")
            {
                human = PlayerColour::blue;
                computer = PlayerColour::red;
                cout << "OK! Human is Blue(" << human << "). Computer is Red(" << computer << ")\n";
                return false;
            }

            cout << "Invalid Colour! Please try again\n";
            return true;
        }

        // Update nodes and neighbours
        void updateNodesNeighbours(pair<int, int> &coords,
                                   vector<int> &converted_neighbours, vector<int> &node_vec,
                                   unordered_map<int, vector<int>> &map)
        {
            // Add the coord as an int
            int node = convertCoordToInt(coords);
            node_vec.push_back(node);

            //Add to neighbours
            //First determine if the coord exist in neighbours
            if (map.find(node) == map.end())
            {
                map[node] = vector<int>();
            }

            // vector<int> &realised_neighbours = blue_neighbours[]
            for (auto x : converted_neighbours)
            {
                // Add to neighbours if any of the neighbour is in the node set
                if (map.find(x) != map.end())
                {
                    // only add to existing if not present
                    vector<int> &n = map[node];
                    if (find(n.begin(), n.end(), x) == n.end())
                        n.push_back(x);

                    vector<int> &xvec = map[x];
                    if (find(xvec.begin(), xvec.end(), node) == xvec.end())
                        xvec.push_back(node);
                }
            }
        }

        // Neighbours
        vector <pair<int,int>> getNeighbours(pair<int,int> x) //get valid neighbours from entry
        {
            vector<pair<int, int>> neighbours;

            // Add pairs depending on position
            if ( (x.first == 0) && (x.second == 0) ) // top left
            {
                pair<int, int> p1(0, 1), p2(1, 0);
                neighbours.push_back(p1);
                neighbours.push_back(p2);
            }
            else if ( (x.first == board_size - 1) && (x.second == board_size - 1) ) // bottom right
            {
                pair<int, int> p1(board_size - 1, board_size - 2), p2(board_size - 2, board_size - 1);
                neighbours.push_back(p1);
                neighbours.push_back(p2);
            }
            else if ((x.first == 0) && (x.second == board_size - 1)) // top right
            {
                pair<int, int> p1(0, board_size - 2), p2(1, board_size - 2), p3(1, board_size - 1);
                neighbours.push_back(p1);
                neighbours.push_back(p2);
                neighbours.push_back(p3);
            }
            else if ((x.first == board_size - 1) && (x.second == 0)) // bottom left
            {
                pair<int, int> p1(board_size - 2, 0), p2(board_size - 2,1), p3(board_size - 1,1);
                neighbours.push_back(p1);
                neighbours.push_back(p2);
                neighbours.push_back(p3);
            }
            else if ((x.first == 0)) // top
            {
                pair<int, int> p1(0, x.second - 1), p2(0, x.second + 1), p3(1, x.second - 1), p4(1, x.second);
                neighbours.push_back(p1);
                neighbours.push_back(p2);
                neighbours.push_back(p3);
                neighbours.push_back(p4);
            }
            else if ((x.first == board_size - 1)) // bottom
            {
                pair<int, int> p1(board_size - 1, x.second - 1), p2(board_size - 1, x.second + 1), p3(board_size - 2, x.second), p4(board_size - 2, x.second + 1);
                neighbours.push_back(p1);
                neighbours.push_back(p2);
                neighbours.push_back(p3);
                neighbours.push_back(p4);
            }
            else if ((x.second == 0)) // left
            {
                pair<int, int> p1(x.first - 1, 0), p2(x.first + 1, 0), p3(x.first - 1, 1), p4(x.first, 1);
                neighbours.push_back(p1);
                neighbours.push_back(p2);
                neighbours.push_back(p3);
                neighbours.push_back(p4);
            }
            else if ((x.second == board_size - 1)) // right
            {
                pair<int, int> p1(x.first - 1, board_size - 1), p2(x.first + 1, board_size - 1), p3(x.first, board_size - 2), p4(x.first + 1, board_size - 2);
                neighbours.push_back(p1);
                neighbours.push_back(p2);
                neighbours.push_back(p3);
                neighbours.push_back(p4);
            }
            else
            {
                // top row
                neighbours.push_back(pair<int, int>(x.first - 1, x.second));
                neighbours.push_back(pair<int, int>(x.first - 1, x.second +1));

                // top row
                neighbours.push_back(pair<int, int>(x.first, x.second - 1));
                neighbours.push_back(pair<int, int>(x.first, x.second + 1));
                
                // bottom row
                neighbours.push_back(pair<int, int>(x.first + 1, x.second - 1));
                neighbours.push_back(pair<int, int>(x.first + 1, x.second));
            }

            // cout << "\nNeighbours of (" << x.first << ", " << x.second << ") is:\n";
            // for (auto p : neighbours)
            //     cout << p;
            // cout << endl;

            return neighbours;
        }

        // Computer move
        virtual pair<int, int> computerMove()
        {
            //what colour is the computer
            vector<int> nodevec;
            unordered_map<int, vector<int>> neighbours;

            if(computer==PlayerColour::blue)
            {
                nodevec = blue_nodes;
                neighbours = blue_neighbours;
            }
            else
            {
                nodevec = red_nodes;
                neighbours = red_neighbours;
            }

            // get the paths
            int longest_path_length=-1; // the length of paths
            vector<int> longest_path;
            // cout << "COMPUTER MOVES:\n";
            // cout << "NODEVEC:";
            // for (auto a : nodevec)
            //     cout << a << " ";
            // cout << endl;

            // cout << "blue neighbours:";
            // for(auto n: neighbours)
            // {
            //     cout << n.first << ":";
            //     for (auto a : n.second)
            //         cout << a << " ";
            // }
            // cout << endl << "------------------" << endl;
            // cout << "nodevec size: " << nodevec.size() << endl;
            
            if(nodevec.size() == 0)
            {
                int i = board_size / 2,j;
                j = i;
                bool not_valid = true;

                while (not_valid)
                    if(board[i][j] == PlayerColour::unvisited) // empty
                        return pair<int, int>(i,j);
                    else
                    {
                        // generate random number
                        random_device rd;
                        default_random_engine eng(rd());
                        uniform_real_distribution<float> distr(0, 1);
 
                        if (distr(eng) < 0.5)
                        {
                            if (computer == PlayerColour::blue)
                                return pair<int, int>(i-1, j);
                            else
                                return pair<int, int>(i, j-1);
                        }
                        else
                        {
                            if (computer == PlayerColour::blue)
                                return pair<int, int>(i + 1, j);
                            else
                                return pair<int, int>(i , j + 1);
                        }
                        
                    }
                    
                
            }
            else
            {
                if (nodevec.size()==1)
                {
                    longest_path_length = 1;
                    longest_path = nodevec;
                }
                else
                    for (int i = 0; i < nodevec.size() - 1; i++)
                        for (int j = i + 1; j < nodevec.size(); j++)
                        {
                            // cout << i << ":" << endl;

                            pair<int, vector<int>> d = dijkstraPathLength(true, nodevec[i], nodevec[j], nodevec, neighbours);

                            // cout << "DIJKSTRA:";
                            // for (auto aa : d.second)
                            // {
                            //     cout << aa << " ";
                            // }
                            // cout << endl;

                            if (d.first > longest_path_length)
                            {
                                longest_path_length = d.first;
                                longest_path = d.second;
                            }
                        }
            }
                    
            // cout << "LONGEST PATH:";
            // for(auto a: longest_path)
            //     cout << a << "--";
            // cout << endl;

            if (computer == PlayerColour::blue)
            {
                // choose the left or right column with the most space
                int min_col = board_size, max_col = -1, used_node_left, used_node_right;
                for (auto a : longest_path)
                {
                    int mod_num = a % board_size;

                    // cout << "NODE:" << a << " MODNUM:" << mod_num << endl;
                    
                    if (mod_num < min_col)
                    {
                        min_col = mod_num;
                        used_node_left = a;
                    }

                    if (mod_num > max_col)
                    {
                        max_col = mod_num;
                        used_node_right = a;
                    }

                }
                // cout << "MINCOL:" << min_col << " MAXCOL:" << max_col << endl;

                // if on the edge, return the othe side
                // if(max_col == board_size -1)
                // {
                //     return pair<int, int>((used_node_left-1) / board_size, (used_node_left-1) % board_size);
                // }
                // if (min_col == 0)
                // {
                //     return pair<int, int>((used_node_right + 1) / board_size, (used_node_right + 1) % board_size);
                // }

                int space_left = min_col - 0, space_right = board_size - 1 - max_col;

                // cout << "SPACE LEFT:" << space_left << " SPACE RIGHT:" << space_right << endl;

                if (space_left < space_right)
                {
                    int ro, col;
                    ro = (used_node_right + 1) / board_size;
                    col = (used_node_right + 1) % board_size;

                    // cout << " MORE SPACE TO THE RIGHT:(" << ro << "," << col << ")";

                    // select extension randomly
                    vector<int> possible_states = {max<int>(0, ro - 1), ro, min<int>(board_size - 1, ro + 1)};
                    return randomExtension(possible_states, true, col);
                }
                else
                {
                    int ro, col;
                    ro = (used_node_left - 1) / board_size;
                    col = (used_node_left - 1) % board_size;

                    // cout << " MORE SPACE TO THE LEFT(" << ro << "," << col << ")";

                    // select extension randomly
                    vector<int> possible_states = {max<int>(0, ro - 1), ro, min<int>(board_size - 1, ro + 1)};
                    return randomExtension(possible_states, true, col);
                }
            }
            else
            {
                // choose the left or right column with the most space
                int min_row = board_size, max_row = -1, used_node_top, used_node_bottom;
                for (auto a : longest_path)
                {
                    int row = a / board_size;

                    // cout << "NODE:" << a << endl;

                    if (row < min_row)
                    {
                        min_row = row;
                        used_node_top = a;
                    }

                    if (row > max_row)
                    {
                        max_row = row;
                        used_node_bottom = a;
                    }
                }

                // cout << "MINROW:" << min_row << " MAXROW:" << max_row << endl;

                int space_top = min_row - 0, space_bottom = board_size - 1 - max_row;

                // cout << "SPACE TOP:" << space_top << " SPACE BOTTOM:" << space_bottom << endl;

                if (space_top > space_bottom)
                {
                    int ro, col;
                    ro = used_node_top / board_size - 1;
                    col = used_node_top % board_size;

                    // cout << " MORE SPACE TO THE TOP:(" << ro << "," << col << ")";

                    // select extension randomly
                    vector<int> possible_states = {max<int>(0, col - 1), col, min<int>(board_size - 1, col + 1)};
                    return randomExtension(possible_states, false, ro);
                }
                else
                {
                    int ro, col;
                    ro = used_node_bottom / board_size + 1;
                    col = used_node_bottom % board_size;

                    // cout << " MORE SPACE TO THE BOTTOM:(" << ro << "," << col << ")";

                    // select extension randomly
                    vector<int> possible_states = {max<int>(0, col - 1), col, min<int>(board_size - 1, col + 1)};
                    return randomExtension(possible_states, false, ro);
                }
            }
            
            
            // cout << "ALL DONE\n";

            return pair<int, int>();
        }


        // return randomised extension
        pair<int, int> randomExtension(vector<int> possible_states, bool columns_fixed, int row_column)
        {
            // random number engine
            random_device rd;
            default_random_engine eng(rd());
            uniform_real_distribution<float> distr(0, 1);

            if (distr(eng) < 0.4)
            {
                iter_swap(possible_states.begin(), possible_states.begin() + 1);
            }
            else if (distr(eng) < 0.8)
            {
                iter_swap(possible_states.begin(), possible_states.begin() + 1);
                iter_swap(possible_states.begin()+1, possible_states.begin() + 2);
            }
            else
            {
                iter_swap(possible_states.begin(), possible_states.begin() + 2);
            }

            
            // cout << "PPOSSIBLE STATES:\n";
            // for (auto a : possible_states)
            //     cout << a << " ";
            // cout << endl;

            // cout << "COLUMNSFIXED:" << columns_fixed << endl;
            // cout << "row_column:" << row_column << endl;

            while (possible_states.size() > 0)
            {
                int i;
                i = possible_states[0];

                // cout << "CHECKING:" << row_column << ", " << i << endl;

                if (columns_fixed)
                {
                    if (board[i][row_column] == PlayerColour::unvisited)
                        return pair<int, int>(i, row_column);
                }
                else
                {

                    if (board[row_column][i] == PlayerColour::unvisited)
                        return pair<int, int>(row_column, i);
                }

                possible_states.erase(possible_states.begin());
                }

            // if this fails, randomly enter a position
            while (true)
            {
                vector<int> available_positions;
                for (int i = 0; i < board_size; i++)
                    for (int j = 0; j < board_size; j++)
                        if (board[i][j] == PlayerColour::unvisited)
                            available_positions.push_back(i * board_size + j);

                int random_index = distr(eng) * board_size;

                int i, j;
                i = available_positions[random_index] / board_size;
                j = available_positions[random_index] % board_size;
                return pair<int, int>(i, j);
            }

            return pair<int, int>();
        }

        //check if spaces left
        bool spaceLeft()
        {
            for(int i = 0; i < board_size ; i ++)
                for (int j = 0; j < board_size; j++)
                    if(board[i][j] == PlayerColour::unvisited)
                        return true;
            return false;
        }

        //Print a board
        string printBoard(vector<vector<PlayerColour>> board)
        {
            stringstream os;
            // The board
            for (int i = 0; i < board.size() - 1; i++)
            {
                vector<PlayerColour> row = board[i];
                os << row;

                // advance by 1 row
                // the edges
                for (int j = 0; j <= i; j++)
                    os << "  ";
                for (int j = 0; j < row.size() - 1; j++)
                    os << "\\ / ";
                os << "\\" << endl;

                // space for next row
                for (int j = 0; j <= i; j++)
                    os << "  ";
            }

            // last row
            os << board[board.size() - 1];

            return os.str();
     }
};

// ----------
// AdvancedHex
class AdvancedHex: public Hex{
    public:
        AdvancedHex(int board_size = 7, int monte_carlo_runs=1000) : Hex(board_size), monte_carlo_runs(monte_carlo_runs)
        {
            // generate random number
            random_device rd;
            eng = default_random_engine(rd());
            distr = uniform_real_distribution<float>(0, 1);
        }

protected:
    // random engine
    default_random_engine eng;
    uniform_real_distribution<float> distr;
    int monte_carlo_runs;

    pair<int, int> computerMove(); // Computer move
    pair<int, int> monteCarloRun(vector<int> free_nodes); // Monte Carlo Run
    vector<int> getFreeNodes(); //convert free positions on board to vector
};

// // Computer move - using MC
pair<int, int> AdvancedHex::computerMove()
{
    cout << "...COMPUTER THINKING...\n";
    vector<int> free_nodes = getFreeNodes();
    vector<int> wins_(free_nodes.size(), 0);

    for (int i = 0; i < monte_carlo_runs; ++i)
    {
        // cout << i << "-";
        pair<int, int> result = monteCarloRun(free_nodes);

        auto ix_ptr = find(free_nodes.begin(), free_nodes.end(), result.second);
        int ix = ix_ptr - free_nodes.begin();

        wins_[ix] = wins_[ix] + result.first;
    }

    // cout << "result:";
    // for (int i : wins_)
    //     cout << i << " ";
    // cout << endl;
    // cout << "free_nodes:";
    // for (int i : free_nodes)
    //     cout << i << " ";
    // cout << endl;

    int max_el = *max_element(wins_.begin(), wins_.end());

    // cout << "max:" << max_el << endl;

    // vector<int> candidates;
    pair<int, int> ret;

    for (int j = 0; j < wins_.size(); j++)
        if (wins_[j] == max_el)
        {
            // candidates.push_back(j);
            ret = pair<int, int>(free_nodes[j] / board_size, free_nodes[j] % board_size);
            // cout << free_nodes[j] << " - (" << ret.first << "," << ret.second << ")\n";
            return ret;
        }

    // cout << "candidates:";
    // for (auto j : candidates)
    //     cout << j << " ";
    // cout << endl;

    return ret;
}

// Monte Carlo Run
pair<int, int> AdvancedHex::monteCarloRun(vector<int> free_nodes)
{

    // // Make some copies of the current state
    // updateNodesNeighbours(pair<int, int> & coords,
    //                       vector<int> & converted_neighbours, vector<int> & node_vec,
    //                       unordered_map<int, vector<int>> & map);

    vector<vector<PlayerColour>> copy_of_board(board); // copy current state of the board
    vector<int> node_vec;
    unordered_map<int, vector<int>> map;


    if (blue_moves)
    {
        node_vec = vector<int>(blue_nodes);
        map = unordered_map<int, vector<int>>(blue_neighbours);
    }
    else
    {
        node_vec = vector<int>(red_nodes);
        map = unordered_map<int, vector<int>>(red_neighbours);
    }

    // setup local version of the blue/not blue move flag
    bool mc_blue_moves = blue_moves,
         first_move;
    int n = free_nodes.size(), random_ix, first_move_node;

    float equal_prob;
    first_move = true;

    // cycle through all remaining nodes, making random plays
    while (n > 0)
    {

        // equal prob of placing
        equal_prob = 1.0 / n;
        random_ix = distr(eng) * n;

        int node_to_remove = free_nodes[random_ix]; // choose random placement
        // save the very next move
        if (first_move)
            first_move_node = node_to_remove;

        pair<int, int> coords(node_to_remove / board_size, node_to_remove % board_size); // as coords

        //only update for the relevant colour
        if (mc_blue_moves == blue_moves)
        {
            // And add neighbours
            vector<pair<int, int>> neighbours = getNeighbours(coords);
            vector<int> converted_neighbours;
            for (auto n : neighbours)
                converted_neighbours.push_back(convertCoordToInt(n));

            // Add blue and red graph info
            updateNodesNeighbours(coords, converted_neighbours, node_vec, map);
        }

        // Place on board
        if (mc_blue_moves)
            copy_of_board[coords.first][coords.second] = PlayerColour::blue;
        else
            copy_of_board[coords.first][coords.second] = PlayerColour::red;

        //switch
        mc_blue_moves = !mc_blue_moves;

        //remove index
        free_nodes.erase(free_nodes.begin() + random_ix);

        // reduce the number of elements
        n--;

        first_move = false;

        // cout << "Freenodes size: " << free_nodes.size() << " n:" << n << " some rand num:" << random_ix << endl;
        // cout << first_move_node << " " << node_to_remove << " Free nodes:";
        // for (auto n : free_nodes)
        //     cout << n << "-";
        // cout << endl;
    }

    int win_lose = checkWin(blue_moves, node_vec, map);
    // cout << printBoard(copy_of_board);
    // cout << "\n";
    // cout << "WIN:" << win_lose << endl;

    return pair<int,int>(win_lose,first_move_node);
}

//convert free positions on board to vector
vector<int> AdvancedHex::getFreeNodes()
{
    vector<int> positions;
    for(int i=0; i < board_size; i++)
        for(int j = 0; j < board_size ; j++)
            if(board[i][j] == PlayerColour::unvisited)
                positions.push_back(board_size * i + j);

    // cout << "Free nodes:";
    // for (auto n : positions)
    //     cout << n << "-";
    // cout << endl;

    return positions;
}

// -------------------------------
// -------------------------------
// MAIN PROGRAM

int main()
{
    AdvancedHex h(11,5000);
    h.reset();
    h.run();
    // return 0 by default on success
    return 0;
}