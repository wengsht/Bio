
#define IMG_MARGIN 1
/* Octaves  */
#define DEFAULT_BASIC_SIGMA 1.6
#define DEFAULT_INIT_SIGMA 0.5

/* number of layers per octave */
// Pls, don't set value higher than range(EXTREMA_FLAG_TYPE) - 1 -3
#define DEFAULT_INNER_LAYER_OCT 3
/*  Type for layer flag  */
#define EXTREMA_FLAG_TYPE unsigned char

/* D(x), all extrema with a value less than 0.04 will be discarded */
#define DEFAULT_EXTREMA_THRESHOLD 0.04

// r Tr^2/Det < (r+1)^2 / r
#define DEFAULT_EDGE_POINT_THRESHOLD 10

#define DEFAULT_DESCR_LEN 128

#define DEFAULT_ORI_HIST_BINS 36

#define DEFAULT_ORI_WIN_RADIUS 3

#define DEFAULT_ORI_SMOOTH_TIMES 2

#define DEFAULT_ORI_SIGMA_TIMES 2

#define DEFAULT_ORI_DEN_THRES 0.8

#define EXTREMA_OFFSET_STEP 5

#define DEFAULT_OFFSET_THRES 0.5

#define DEFAULT_DESCRIPTOR_WIN_WIDTH 4

#define DEFAULT_DESCRIPTOR_SCALE_ADJUST 3

#define DEFAULT_DESCRIPTOR_HIST_BINS 8

// D(x) threshold
#define DEFAULT_DX_VALUE_THRES 0.04

// octIdx of octave with same size as original img
#define DEFAULT_BASE_OCT_IDX 1
