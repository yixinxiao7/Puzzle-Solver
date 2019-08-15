#include <getopt.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include "PuzzleSolver.h"
#include <deque>

using namespace std;

//only called once, time not a problem
void fill_color_vec(int num_colors, vector<char> &colors) {
	vector<char> alphabet;
	alphabet.reserve(27);
	alphabet.push_back('^');
	char character = 'a';
	while (num_colors != 0) {
		alphabet.push_back(character);
		++character; 
		--num_colors;
	}
	colors = alphabet;
}

//only called once, time not a problem
void PuzzleSolver::read_map(vector<vector<char> > &map) {
	char symbol;
	string skipped_line;
	
	unsigned int r = 0;
	unsigned int c = 0;
	while (cin >> symbol) {
		if (symbol == '/') {
			getline(cin, skipped_line);
		}
		else {		
			map[r][c] = symbol;
			if (c == width - 1) {
				++r; 
				c = 0;
			}
			else {
				++c;
			}
		}//else
	}//while
}//end

void printHelp(char *argv[]) {
	cout << "This is the first argument: " << argv[0] << "\n";
	cout << "pls work" << "\n";
} 

void options(int argc, char * argv[], PuzzleSolver &ps) {
	char collection_type = 'x';
	string output_type = "default";
	
	opterr = true;
	int choice;
	int option_index = 0;
	option long_options[] = {
		{ "help",  no_argument, nullptr, 'h'},
		{ "queue", no_argument, nullptr, 'q'},
		{ "stack", no_argument, nullptr, 's'},
		{ "output", required_argument, nullptr, 'o'},
		{ nullptr, 0, nullptr, '\0' }
	};

	while ((choice = getopt_long(argc, argv, "o:hqs", long_options, &option_index)) != -1) {
		switch (choice) {
		case 'h':
			printHelp(argv);
			exit(0);
		case 'q':
			if (collection_type != 'x') {
				cerr << "Error: You must input either stack or queue, not both." << '\n';
				exit(1);
			}
			collection_type = 'q';
			break;
		case 's':
			if (collection_type != 'x') {
				cerr << "Error: You must input either stack or queue, not both." << '\n';
				exit(1);
			}
			collection_type = 's';
			break;
		case 'o':
			output_type = optarg;
			if (output_type != "list" && output_type != "map") {
				cerr << "Error: invalid output option" << '\n';
				exit(1);
			}
			break;
		default:
			cerr << "Error: invalid option" << '\n';
			exit(1);
		} //switch
	} // while
	if (collection_type != 'q' && collection_type != 's') {
		cerr << "Error: invalid collection type option" << '\n';
		exit(1);
	}
	if (output_type == "default") {
		output_type = "map";
	}
 	ps.set_collection_type(collection_type);
  	ps.set_output_type(output_type);
}

//constant time
bool is_color(char c) {
	unsigned int int_c = (int) c;
	if ((int_c >= 97 && int_c <=122) || (int_c >=65 && int_c <= 90) || int_c == 94) {
		return true;
	}
	return false;
}

//linear time, ran only in checking function
bool is_legal_color(char c, const vector<char> &color) {
	for (size_t i = 0; i < color.size(); ++i) {
		char door = (char)(color[i] - 32);

		if (color[i] == '^') {
			door = '^';
		}

		if (c == color[i] || c == door) {
			return true;
		}
	}
	return false;
}

//constant time
bool is_color_nonvector(char c, char color) {
		char door = (char)(color - 32);
		if (color == '^') {
			door = '^';
		}

		if (c == color || c == door) {
			return true;
		}
	return false;
}

//linear time
bool check_legal(const PuzzleSolver &model, const vector<vector<char> > &map, const vector<char> &color) {
	bool pass = true;
	int n_c = model.get_num_colors();
	int w = model.get_width();
	int h = model.get_height();
	if (n_c < 0 || n_c > 26) {
		pass = false;
	}
	if (w < 1 || h < 1) {
		pass = false;
	}
	//check map
	unsigned int start_counter = 0;
	unsigned int end_counter = 0;
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			char c = map[i][j];
			if (c == '@') {
				++start_counter;
			}
			else if (c == '?') {
				++end_counter;
			}
			if (c != '.' && c != '#' && c != '?' && c != '@') {
				if(!is_legal_color(c, color)){
					pass = false;
				}
			}
		}
	}
	if (start_counter != 1 || end_counter != 1) {
		pass = false;
	}
	return pass;
}

//constant time
bool is_button(char c) {
	unsigned int int_c = (int) c;
	if ((int_c >= 97 && int_c <= 122) || int_c == 94) {
		return true;
	}
	return false;
}

//REQUIRES: ALL data structures should already be initialized 
bool solve_puzzle(const vector<vector<char> > &map, vector<vector<vector<Tile> > > &backtrack, PuzzleSolver &model, unsigned int &counter) {
	unsigned int start_row = 0;
	unsigned int start_col = 0;
	unsigned int map_height = model.get_height();
	unsigned int map_width = model.get_width();

	for (unsigned int i = 0; i < map_height; ++i) {
		for (unsigned int j = 0; j < map_width; ++j) {
			if (map[i][j] == '@') {
				start_row = i;
				start_col = j;
			}
		}
	}
	backtrack[0][start_row][start_col].direction = ',';
	++counter;
	State start = { start_row, start_col, '^' };
	model.reachable_collection.push_back(start);
	State S;
	bool beatable = false;



	char current_col = '^';

	while (!(model.reachable_collection.empty())) {
		S = { 0, 0 , '^' };//filler
		if (model.get_collection_type() == 'q') {
			S = model.reachable_collection.front();
			model.reachable_collection.pop_front();
		}
		else if (model.get_collection_type() == 's') {
			S = model.reachable_collection.back();
			model.reachable_collection.pop_back();
		}

		unsigned int c_r = S.row;
		unsigned int c_c = S.col;
		current_col = S.color;
		unsigned int current_col_index = current_col - 'a' + 1;
			
		if (current_col == '^') {
			current_col_index = 0;
		}

		bool isColor = false;
			
		char current_spot = map[c_r][c_c];
		int next_spot = current_spot - 'a' + 1; 
		if (current_spot == '^') {
			next_spot = 0;
		}
			
		if (is_button(current_spot) && current_spot != current_col) {
			isColor = true;
			if (backtrack[next_spot][c_r][c_c].direction == '~') {
				State next_state = { c_r, c_c, current_spot };
				model.reachable_collection.push_back(next_state);
				backtrack[next_spot][c_r][c_c].direction = current_col;
				++counter;
			}
		}//if
		if (!isColor) { 
			char door = (char) (current_col - 32);
			if (current_col == '^') {
				door = '^';
			}
			if (c_r != 0) {
				char north_spot = map[c_r - 1][c_c];
				if (north_spot != '#' && backtrack[current_col_index][c_r - 1][c_c].direction == '~') {
					if (north_spot == '.' || north_spot == door || is_button(north_spot) || north_spot == '?' || north_spot == '@') {//can simplify is_button and door im pretty sure tho
						if (north_spot == '?') {
								
							backtrack[current_col_index][c_r - 1][c_c].direction = '(';
							++counter;
							beatable = true;
							break;
						}
						State north = { c_r - 1, c_c, current_col };
						model.reachable_collection.push_back(north);
						backtrack[current_col_index][c_r - 1][c_c].direction = 'D';
						++counter;
					}
				}
			}//if north
			if (c_c != map_width - 1) {
				char east_spot = map[c_r][c_c + 1];
				if (east_spot != '#' && backtrack[current_col_index][c_r][c_c + 1].direction == '~') {
					if (east_spot == '.' || east_spot == door || is_button(east_spot) || east_spot == '?' || east_spot == '@') {
						if (east_spot == '?') {
								
							backtrack[current_col_index][c_r][c_c + 1].direction = ')';
							++counter;
							beatable = true;
							break;
						}
						State east = { c_r, c_c + 1, current_col };
						model.reachable_collection.push_back(east);
							
						backtrack[current_col_index][c_r][c_c + 1].direction = 'L';
						++counter;
					}//if	
				}//if
			}//if east
			if (c_r != map_height - 1){
				char south_spot = map[c_r + 1][c_c];
				if (south_spot != '#' && backtrack[current_col_index][c_r + 1][c_c].direction == '~') {
					if (south_spot == '.' || south_spot == door || is_button(south_spot) || south_spot == '?' || south_spot == '@') {//make criteria function for this TODAY
						if (south_spot == '?') {
								
							backtrack[current_col_index][c_r + 1][c_c].direction = '[';
							++counter;
							beatable = true;
							break;
						}
						State south = { c_r + 1, c_c, current_col };
						model.reachable_collection.push_back(south);
							
						backtrack[current_col_index][c_r + 1][c_c].direction = 'U';
						++counter;
					}//if
				}//if
			}//if south
			if (c_c != 0) {
				char west_spot = map[c_r][c_c - 1];
				if (west_spot != '#' && backtrack[current_col_index][c_r][c_c - 1].direction == '~') {
					if (west_spot == '.' || west_spot == door || is_button(west_spot) || west_spot == '?' || west_spot == '@') {//make criteria function for this TODAY
						if (west_spot == '?') {
								
							backtrack[current_col_index][c_r][c_c - 1].direction = ']';
							++counter;
							beatable = true;
							break;
						}
						State west = { c_r, c_c - 1, current_col };
						model.reachable_collection.push_back(west);
							
						backtrack[current_col_index][c_r][c_c - 1].direction = 'R';
						++counter;
					}//if
				}//if
			}//if west
		}//if
	}//while
	return beatable;
}

void print_map( const vector<vector<char> > &map, vector<vector<vector<Tile> > > &backtrack, const vector<char> &color, const PuzzleSolver &model, unsigned int counter) {
	unsigned int n_c = model.get_num_colors() + 1;
	unsigned int h = model.get_height();
	unsigned int w = model.get_width();
	
	vector<State> print;
	print.reserve((counter + 2));
	unsigned int c_r = 0;//filler
	unsigned int c_c = 0;//filler
	char current_col = '^';//filler

	for (unsigned int i = 0; i < n_c; ++i) {
		for (unsigned int j = 0; j < h; ++j) {
			for (unsigned int k = 0; k < w; ++k) {
				char start_direction = backtrack[i][j][k].direction;
				if (start_direction == '(') {
					State begin = { j, k, color[i] };
					print.push_back(begin); 
					c_r = j + 1;
					c_c = k;
					current_col = color[i];
				}
				else if (start_direction == ')') {
					State begin = { j, k, color[i] };
					print.push_back(begin);
					c_r = j;
					c_c = k - 1;
					current_col = color[i];
				}
				else if (start_direction == '[') {
					State begin = { j, k, color[i] };
					print.push_back(begin);
					c_r = j - 1;
					c_c = k;
					current_col = color[i];
				}
				else if (start_direction == ']') {
					State begin = { j, k, color[i] };
					print.push_back(begin);
					c_r = j;
					c_c = k + 1;
					current_col = color[i];
				}//if
			}
		}
	}//triple for
	
	State current = { c_r, c_c, current_col };
	unsigned int current_col_index = current_col - 'a' + 1;
	if (current_col == '^') {
		current_col_index = 0;
	}
	while (backtrack[current_col_index][c_r][c_c].direction != ',') {
		current = { c_r, c_c, current_col };
		current_col_index = current_col - 'a' + 1;
		if (current_col == '^') {
			current_col_index = 0;
		}
		char c_d = backtrack[current_col_index][c_r][c_c].direction;
		print.push_back(current);
		if (c_d == 'D') {
			c_r = c_r + 1;
		}
		else if (c_d == 'L') {
			c_c = c_c - 1;
		}
		else if (c_d == 'U') {
			c_r = c_r - 1;
		}
		else if (c_d == 'R') {
			c_c = c_c + 1;
		}
		else if (c_d != '[' && c_d != ']' && c_d != '(' && c_d !=  ')' && c_d != '~' && c_d != '!') {
			current_col = c_d;
		}
	}//while
	State begin = { c_r, c_c, '^' };
	print.push_back(begin);

	if (model.get_output_type() == "list") {
		for (int i = (int)print.size() - 1; i >= 0; --i) {
			cout << "(" << print[i].color << ", (" << print[i].row << ", "
				<< print[i].col << "))" << '\n';
		}
	}//if list

	if (model.get_output_type() == "map") {
		
		unsigned int r = 0;
		unsigned int c = 0;
		char current_col = print[print.size() - 1].color;

		while (r != h) {
			
			for (unsigned int i = 0; i < n_c; ++i) {
				current_col = color[i];
				char map_spot = map[r][c];
				if (is_color_nonvector(map_spot, current_col)) {
					backtrack[i][r][c].direction = '.';
				}
				else if (map_spot == '@') {
					if (current_col != '^') {
						backtrack[i][r][c].direction = '.';
					}
					else {
						backtrack[i][r][c].direction = '@';
					}
				}
				else {
					backtrack[i][r][c].direction = map_spot;
				}	
			}//for
			++c;
			if (c == w) {
				c = 0;
				++r;
			}
		}

		for (int i = (int)print.size() - 1; i >= 0; --i) {
			unsigned int r = print[i].row;
			unsigned int c = print[i].col;
			char current_col = print[i].color;
			int current_col_index = current_col - 'a' + 1;
			if (current_col == '^') {
				current_col_index = 0;
			}
			if (is_button(backtrack[current_col_index][r][c].direction)) {
				backtrack[current_col_index][r][c].direction = '%';
				unsigned int next_index = map[r][c] - 'a' + 1;
				if (map[r][c] == '^') {
					next_index = 0;
				}
				backtrack[next_index][r][c].direction = '@';

			}
			else if (backtrack[current_col_index][r][c].direction != '@' && backtrack[current_col_index][r][c].direction != '?') {
				backtrack[current_col_index][r][c].direction = '+';
			}
		}//for
		string to_print = "";
				
		for (unsigned int i = 0; i < n_c; ++i) {
			cout << "// color " << color[i] << '\n';
			for (unsigned int j = 0; j < h; ++j) {
				for (unsigned int k = 0; k < w; ++k) {
					to_print += backtrack[i][j][k].direction;
				}//end column
				cout << to_print << '\n';
				to_print.clear();
			}//row
		}//color
		cout << '\n';
	}//if map
}

void print_unreachable_map(vector<vector<char> > &map, const vector<vector<vector<Tile> > > &backtrack, const PuzzleSolver &model) {
	int row = 0;
	int col = 0;
	int n_c = model.get_num_colors() + 1;
	int w = model.get_width();
	int h = model.get_height();
	while (row != h) {
		bool place_wall = true;
		for (int i = 0; i < n_c; ++i) {
			char c_d = backtrack[i][row][col].direction;
			if (c_d != '~') {
				place_wall = false;
			}
		}
		if (place_wall) {
			map[row][col] = '#';
		}
		++col;
		if (col == w) {
			++row;
			col = 0;
		}
	}//while
	
	//print
	cout << "No solution." << '\n';
	cout << "Reachable:" << '\n';
	for (int i = 0; i < model.get_height(); ++i) {
		for (int j = 0; j < model.get_width(); ++j) {
			cout << map[i][j];
		}
		cout << '\n';
	}
	cout << '\n';
}

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);

	PuzzleSolver model; 
	options(argc, argv, model);

	//read in map
	model.read_parameters();
	vector<vector<char> > map(model.get_height(), vector<char>(model.get_width()));
	model.read_map(map);
	
	//initialize color vector
	vector<char> color;
	color.reserve(model.get_num_colors() + 1);//one for ^
	fill_color_vec( model.get_num_colors() ,color);

	bool legal = check_legal(model, map, color);
	if (!legal) {
		cout << "this map gave illegal parameters probably" << '\n';
		exit(1);
	}
	
	Tile filler;
	vector < vector < vector<Tile> > >backtrack((model.get_num_colors() + 1), vector<vector<Tile>>(model.get_height(), vector<Tile>(model.get_width(), filler)));
	unsigned int counter = 0;
	bool passed = solve_puzzle(map, backtrack, model, counter);
	
	if(passed){
		print_map(map, backtrack, color, model, counter);
	}
	else {
		print_unreachable_map(map, backtrack, model);
	}

	return 0;
}