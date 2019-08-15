#include <iostream>
#include <deque>

using namespace std;

struct State {
	unsigned int row;
	unsigned int col;
	char color;
};

struct Tile {
	char direction = '~';//not reachable
};

class PuzzleSolver {
	private:
		string output_type;
		unsigned int num_colors;
		unsigned int height;
		unsigned int width;
		char collection_type;
		
	    
	public:
		deque<State> reachable_collection;

		int get_num_colors() const {
			return num_colors;
		}
		int get_height() const {
			return height;
		}
		int get_width() const {
			return width;
		}
		char get_collection_type() const {
			return collection_type;
		}
		string get_output_type() const {
			return output_type;
		}
		void set_collection_type(char c) {
			collection_type = c;
		}
		void set_output_type(string s) {
			output_type = s;
		}
		//reads and initializes parameters
		void read_parameters() {
			int par_val;
			cin >> par_val;
			num_colors = par_val;
			cin >> par_val;
			height = par_val;
			cin >> par_val;
			width = par_val;
			string junk;
			getline(cin, junk);
		}
		
		//reads map and returns vector of vector
		void read_map(vector<vector<char> > &map);
		
};
