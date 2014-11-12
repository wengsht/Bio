
#define IMG_MARGIN 1
/* Octaves  */
#define DEFAULT_BASIC_SIGMA 1.6

/* number of layers per octave */
// Pls, don't set value higher than range(EXTREMA_FLAG_TYPE) - 1 -3
#define DEFAULT_INNER_LAYER_OCT 3
/*  Type for layer flag  */
#define EXTREMA_FLAG_TYPE unsigned char

/* D(x), all extrema with a value less than 0.03 will be discarded */
#define DEFAULT_EXTREMA_THRESHOLD 0.03

// r Tr^2/Det < (r+1)^2 / r
#define DEFAULT_EDGE_POINT_THRESHOLD 10

#define DEFAULT_DESCR_LEN 128

#define DEFAULT_ORI_HIST_BINS 36

#define DEFAULT_ORI_WIN_RADIUS 3

#define DEFAULT_ORI_SMOOTH_TIMES 2

#define DEFAULT_ORI_SIGMA_TIMES 2

#define DEFAULT_ORI_DEN_THRES 0.8

