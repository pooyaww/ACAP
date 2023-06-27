#define STREAM // WINDOW, STREAM, BUFFER_SCALAR, BUFFER_VECTORIZED, FREE
#define LANE_SIZE 4
#define VECTOR_SIZE LANE_SIZE
#define TYPE int32_t
#define SAMPLE_SIZE 2048
#define WINDOW_SIZE SAMPLE_SIZE //here sample size means one iteration of graph is enough
#define BUFFER_SIZE SAMPLE_SIZE
