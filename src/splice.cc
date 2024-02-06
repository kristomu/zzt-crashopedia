#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

// This tool takes an input ZZT file and a boards directory. It replaces
// every placeholder board in the ZZT file with the corresponding BRD file
// in the boards directory. This is used for uneditable boards, where the
// board is corrupted to provoke bugs in ZZT. Such boards naturally can't
// be generated in-place by KevEdit and must be inserted by other means.

// Placeholder boards have a board title of "PLACEHOLDER: " followed by
// the board filename.

// Currently this is just a prototype. Beware ugly code.

std::vector<char> read(std::ifstream & infile, size_t num_bytes) {
	if (!infile) {
		throw std::invalid_argument("read: Can't read file!");
	}

	std::vector<char> bytes(num_bytes);
	infile.read(bytes.data(), num_bytes);
	return bytes;
}

// Used for reading a whole file, because the .brd files we insert
// as boards may have a misleading board length variable if they're
// Super Lock boards.
std::vector<char> read(std::string path) {
	size_t file_size = std::filesystem::file_size(path);

	std::ifstream infile(path);

	return read(infile, file_size);
}


void write(std::ofstream & outfile, std::vector<char> bytes) {
	outfile.write(bytes.data(), bytes.size());
}

// Assumes that the seek pointer is just before the board data.
std::vector<char> read_ZZT_board(std::ifstream & infile) {
	// First read two bytes to get the board size. Throw an exception
	// if the size is out of bounds. Then read the remaining bytes.
	// WARNING: This is not endian safe! I'll fix it later if needed.
	short board_length = 0;
	char * length_ptr = (char *)&board_length;
	infile.read(length_ptr, 2);

	if (!infile) {
		return {};
	}

	if (board_length < 0 || board_length > 20000) {
		throw std::runtime_error("Invalid board length!");
	}

	std::vector<char> board(2);
	std::copy(length_ptr, length_ptr + 2, board.begin());

	if (board_length == 0) {
		return board;    // all done
	}

	board.resize(board_length + 2);
	infile.read(board.data() + 2, board_length);

	if (!infile) {
		board.resize(2 + infile.gcount());
	}

	return board;
}

std::string get_board_name(std::vector<char> & board) {
	if (board.size() < 2) {
		return "";
	}

	int name_length = (unsigned char)board[2];

	std::string name(board.begin() + 3, board.begin() + 3 + name_length);

	return name;
}

int main() {
	std::string input_path = "test/testworld.zzt";
	std::string board_directory = "test/boards/";

	std::string output_path = "test/output.zzt";

	// Number of bytes from start till the first board.
	const int HEADER_SIZE = 512;

	std::ifstream infile(input_path.c_str());

	// Deal with the header.
	std::vector<char> header = read(infile, HEADER_SIZE);

	std::vector<std::vector<char> > boards;

	// Read the next boards from the input file one by one.
	// If the title shows the board to be a placeholder, read
	// the .brd file in question and replace the contents
	// to be written to the output file with it.
	
	while (infile) {
		std::vector<char> board = read_ZZT_board(infile);
		if (!infile && board.empty()) {
			continue;
		}
		std::string board_name = get_board_name(board);

		std::cout << "That board's name is\t" <<
			board_name << std::endl;

		std::string placeholder_str = "PLACEHOLDER: ";
		size_t placeholder_idx = board_name.find(placeholder_str);

		if (placeholder_idx != std::string::npos) {
			std::string filename(board_name.begin() + placeholder_idx +
					placeholder_str.size(),
				board_name.end());
			std::cout << "\tFound a placeholder for file ["
				<< filename << "]\n";

			board = read(board_directory + filename);
		}

		boards.push_back(board);
	}

	// Now that we got all the boards, create the output file.

	std::cout << "\nCreating " << output_path << std::endl;

	std::ofstream outfile(output_path);

	write(outfile, header);
	for (const std::vector<char> & board: boards) {
		write(outfile, board);
	}
	std::cout << "All done!\n";
}