#define STREAM //WINDOW, STREAM, BUFFER_SCALAR, BUFFER_VECTORIZED, FREE, CIRCULAR_BUFFER_VECTORIZED,CIRCULAR_INPUT_BUFFER, CIRCULAR_OUTPUT_BUFFER, ASYNCHRONOUS_CIRCULAR_BUFFER
#define LANE_SIZE 4
#define VECTOR_SIZE LANE_SIZE
#define TYPE int32_t
//typedef int32_t TYPE;
//typedef struct {std::vector<int32_t> data = std::vector<int32_t>(5);} TYPE;
#define SAMPLE_SIZE 2048
#define WINDOW_SIZE SAMPLE_SIZE //here sample size means one iteration of graph is enough
#define BUFFER_SIZE SAMPLE_SIZE
