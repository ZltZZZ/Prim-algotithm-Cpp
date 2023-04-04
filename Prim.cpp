#include <iostream>
#include <fstream>

#define TYPE_INT 1
#define TYPE_BOOL 2

using namespace std;

/* Returns false if allocation fails, returns true if allocation success. No need to zero memory, cos of CALLOC. */
bool allocate_matrix(void*** dst, int rank, int type);

/* Free graph matrix. */
void free_matrix(void** dst, int rank, int type);

/* If all fields positive - ostov is full, then return true, else return false. */
bool ostov_is_full(bool* ostov_vertex_list, int** graph, int rank);

/* Check vertex in graph. Returns true, if vertex hasn't vectors (empty), returns true, if has vectors (has neightbors). */
bool vertex_is_empy(int** graph, int num_vertex, int rank);

/* Check, if any 1 in line. Returns true, if there are only zeros, else return true. */
bool line_is_empty(void* line, int rank, int type);

/* If there are vertex, that not in ostov_vertex_list, but in graph that vertex is not empy, than it is new sub grapgh. Return new start num, else return -1. */
int get_next_start(bool* ostov_vertex_list, int** graph, int rank);


int main() {
	ifstream f_in("input.txt");
	ofstream f_out("output.txt");
	int** graph = NULL;
	bool** ostov_map = NULL;
	bool* ostov_vertex_list = NULL;
	int rank = 0, start = 0, var = 0;
	int sum = 0;

	// Open files
	if (!f_in.is_open() || !f_out.is_open()) {
		cout << "error open file\n";
		return -2;
	}

	// Get rank, variant and start vertex for alg
	f_in >> rank >> var >> start;
	if (var != 2) {
		cout << "incorrect var. Expected 2 (Prima)\n";
		return -2;
	}
	if (start < 1 || start > rank) {
		cout << "incorrect start vertex\n";
		return -2;
	}

	// Allocate memory for matrixes
	if (!allocate_matrix((void***)(&graph), rank, TYPE_INT) ||
		!allocate_matrix((void***)(&ostov_map), rank, TYPE_BOOL) ||
		(ostov_vertex_list = (bool*)calloc(rank, sizeof(bool))) == NULL) {
		cout << "allocate fail\n";
		return -2;
	}

	// Get matrix
	for (int i = 0; i < rank; i++) {
		for (int j = 0; j < rank; j++) {
			f_in >> graph[i][j];
		}
	}

	do {
		ostov_vertex_list[start - 1] = 1;

		while (!ostov_is_full(ostov_vertex_list, graph, rank))
		{
			int min = INT_MAX;
			int min_i = -1, min_j = -1;

			// Run through all curr vertexes in ostov
			for (int i = 0; i < rank; i++) {
				if (ostov_vertex_list[i] == 1) { // Find min for all vertexes
					for (int j = 0; j < rank; j++) {
						if (graph[i][j] != 0 && line_is_empty(ostov_map[j], rank, TYPE_BOOL)) { // If beetween i and j vector and j is unique (not in ostov), than do
							if (graph[i][j] < min) { // Remember min vector
								min = graph[i][j];
								min_i = i;
								min_j = j;
							}
						}
					}
				}
			}

			if (min_i == -1 || min_j == -1) { // end of sub graph
				break;
			}

			sum += graph[min_i][min_j];
			ostov_map[min_i][min_j] = 1;
			ostov_map[min_j][min_i] = 1;
			ostov_vertex_list[min_j] = 1;
			if (min_i < min_j) {
				cout << "(" << min_i + 1 << ", " << min_j + 1 << ") ";
			}
			else {
				cout << "(" << min_j + 1 << ", " << min_i + 1 << ") ";
			}
		}

	} while ((start = get_next_start(ostov_vertex_list, graph, rank)) != -1);

	f_out << sum << "\n";
	for (int i = 0; i < rank; i++) {
		for (int j = 0; j < rank; j++) {
			f_out << ostov_map[i][j];
			if (j == rank - 1) {
				f_out << "\n";
			}
			else {
				f_out << ", ";
			}
		}
	}

	free_matrix((void**)graph, rank, TYPE_INT);
	free_matrix((void**)ostov_map, rank, TYPE_BOOL);
	free(ostov_vertex_list);
	f_in.close();
	f_out.close();
	return 0;
}

bool allocate_matrix(void*** dst, int rank, int type) {
	if (type == TYPE_INT) {
		int** ptr = NULL;
		ptr = (int**)calloc(rank, sizeof(int*));
		if (ptr == NULL) return false;

		for (int i = 0; i < rank; i++) {
			ptr[i] = (int*)calloc(rank, sizeof(int));
			if (ptr[i] == NULL) return false;
		}

		*dst = (void**)ptr;
		return true;
	}
	else if (type == TYPE_BOOL) {
		bool** ptr = NULL;
		ptr = (bool**)calloc(rank, sizeof(bool*));
		if (ptr == NULL) return false;

		for (int i = 0; i < rank; i++) {
			ptr[i] = (bool*)calloc(rank, sizeof(bool));
			if (ptr[i] == NULL) return false;
		}

		*dst = (void**)ptr;
		return true;
	}
	else {
		return false;
	}

}

bool ostov_is_full(bool* ostov_vertex_list, int** graph, int rank) {
	for (int i = 0; i < rank; i++) {
		if (ostov_vertex_list[i] == 0 && !vertex_is_empy(graph, i, rank)) {
			return false;
		}
	}

	return true;
}

bool vertex_is_empy(int** graph, int num_vertex, int rank) {
	if (line_is_empty(graph[num_vertex], rank, TYPE_INT)) {
		return true;
	}
	else return false;
}

bool line_is_empty(void* line, int rank, int type) {
	if (type == TYPE_INT) {
		int* line_int = (int*)line;
;		for (int i = 0; i < rank; i++) {
			if (line_int[i] != 0) {
				return false;
			}
		}
	}
	else if (type == TYPE_BOOL){
		bool* line_bool = (bool*)line;
		for (int i = 0; i < rank; i++) {
			if (line_bool[i] != 0) {
				return false;
			}
		}
	}
	else {
		cout << "error line_is_empy : wrong type\n";
		exit(-2);
	}

	return true;
}

void free_matrix(void** dst, int rank, int type) {
	if (type == TYPE_BOOL) {
		bool** ptr = (bool**)dst;

		for (int i = 0; i < rank; i++) {
			free(ptr[i]);
		}

		free(ptr);
	}
	else if (type == TYPE_INT) {
		int** ptr = (int**)dst;

		for (int i = 0; i < rank; i++) {
			free(ptr[i]);
		}

		free(ptr);
	}
	else {
		cout << "Error free_matrix : wrong type\n";
		exit(-2);
	}
}

int get_next_start(bool* ostov_vertex_list, int** graph, int rank) {
	for (int i = 0; i < rank; i++) {
		if (!vertex_is_empy(graph, i, rank) && ostov_vertex_list[i] == 0) {
			return i + 1;
		}
	}

	return -1;
}