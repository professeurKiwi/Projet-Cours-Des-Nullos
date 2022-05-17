//*** CODE PROVENANT DE https://riptutorial.com/algorithm/example/27088/radix-2-fft **//

#define PI       3.1415926535897932384626433832795    // PI for sine/cos calculations
#define TWOPI    6.283185307179586476925286766559     // 2*PI for sine/cos calculations
#define Deg2Rad  0.017453292519943295769236907684886  // Degrees to Radians factor
#define Rad2Deg  57.295779513082320876798154814105    // Radians to Degrees factor
#define log10_2  0.30102999566398119521373889472449   // Log10 of 2 
#define log10_2_INV 3.3219280948873623478703194294948 // 1/Log10(2)

// complex variable structure (double precision)
struct complex
{
public:
    float  Re, Im;        //*** changement du double en float pour économie de mémoire // Not so complicated after all
};

bool isPwrTwo(int N, int *M);

void rad2FFT(int N, float *x, complex *DFT); //*** addaptation pour prendre un tableau de float