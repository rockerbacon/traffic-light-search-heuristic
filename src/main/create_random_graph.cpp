#include "assertions/command_line_interface.h"
#include "traffic_graph/traffic_graph.h"

#include <fstream>

#define DEFAULT_MIN_VERTEX_DEGREE 4
#define DEFAULT_MAX_VERTEX_DEGREE 10
#define DEFAULT_MIN_EDGE_WEIGHT 1
#define UNSPECIFIED_WEIGHT -1

using namespace traffic;
using namespace std;

int main (int argc, char **argv) {

	GraphBuilder *builder;
	ofstream file_stream;

	cli::RequiredArgument<string> output_file_path("output", 'o');
	cli::RequiredArgument<Vertex> number_of_vertices("vertices", 'v');
	cli::RequiredArgument<TimeUnit> cycle("cycle", 'c');
	cli::OptionalArgument<Vertex> min_vertex_degree("min-vertex-degree", DEFAULT_MIN_VERTEX_DEGREE, 'm');
	cli::OptionalArgument<Vertex> max_vertex_degree("max-vertex-degree", DEFAULT_MAX_VERTEX_DEGREE, 'M');
	cli::OptionalArgument<TimeUnit> min_edge_weight("min-edge-weight", DEFAULT_MIN_EDGE_WEIGHT, 'w');
   	cli::OptionalArgument<TimeUnit> max_edge_weight("max-edge-weight", UNSPECIFIED_WEIGHT, 'W');	

	cli::capture_all_arguments_from(argc, argv);

	if (*max_edge_weight == UNSPECIFIED_WEIGHT) {
		*max_edge_weight = *cycle-1;
	}

	builder = new GraphBuilder(*number_of_vertices, *min_vertex_degree, *max_vertex_degree, *min_edge_weight, *max_edge_weight);
	builder->withCycle(*cycle);

	file_stream.open(*output_file_path);
	builder->output_to_file(file_stream);
	file_stream.close();

	delete builder;

	// debugging
#ifdef DEBUG
	GraphBuilder *debug_builder;
	ifstream debug_stream;

	debug_builder = new GraphBuilder();

	debug_stream.open(*output_file_path);
	debug_builder->read_from_file(debug_stream);
	debug_stream.close();

	file_stream.open(*output_file_path+"_debug");
	debug_builder->output_to_file(file_stream);
	file_stream.close();

	delete debug_builder;
#endif


}
