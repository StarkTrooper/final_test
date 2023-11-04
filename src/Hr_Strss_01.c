#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

int HR(int data[], int size) {
    int peaks[2] = {0};
    int hrl[7] = {0};
    int HRs[1] = {4};
    int ctr = 0;   // peaks counter
    int strt = 0;  
    int ctr2 = 0;  // last HR guaranteed

    while (1) {
        //sleep(1);
        
        //printf("%d\n", strt);
        //val = real data
        //hrl[ctr] = val; # for de moment:
        hrl[ctr] = data[strt];
        strt++;
		ctr++;
        while (strt > 5) {
            ctr++;
            //sleep(1);
            int i;
            //printf("%d\n", ctr);          

            if (hrl[3] > hrl[2] && hrl[3] > hrl[4] && hrl[3] > hrl[0] && hrl[3] > hrl[6]) {
                //printf("Ok\n");
                peaks[1] = ctr;
                int dpp = peaks[1] - peaks[0];
                if (dpp > 23) {
                    HRs[0] = 0;
                } else if (dpp < 18) {
                    HRs[0] = 2;
                } else {
                    HRs[0] = 1;
                }
                ctr2++;
                peaks[0]=ctr;
            }

            for (i = 0; i < 6; i++) {
                hrl[i] = hrl[i + 1];
            }
            hrl[6] = data[strt];
            strt++;
            
            if (ctr2 > 1) {
    		return HRs[0];
    	  }
        }
    }
}

void runStressDetectionAlgorithm(int gap, int hr) {
    int lt[5] = {0};
    int ct = 0;
    int i;

    while (1) {
        sleep(1);
        int lt0[6] = {0};

        for (i = 0; i < 6; i++) {
            lt0[i] = rand() % 11;
        }

        int val = lt0[0];
        for (i = 1; i < 6; i++) {
            if (lt0[i] > val) {
                val = lt0[i];
            }
        }

        lt[4] = val;

        if (lt[4] - lt[0] > gap && ct > 4) {
            if (hr == 1) {
                printf("Manual activity (normal HR and hand movement)\n");
            } else if (hr == 0) {
                printf("Danger (constant activity but low HR)\n");
            } else {
                printf("Body activity (constant activity, hence high HR)\n");
            }
        } else if (lt[4] - lt[0] > gap) {
            if (hr == 1) {
                printf("Manual activity (normal HR and high A_G)\n");
            } else if (hr == 0) {
                printf("Manual activity (low HR and high A_G)\n");
            } else {
                printf("Body activity (high HR and high A_G)\n");
            }
        } else {
            if (hr == 1) {
                printf("Normal (Nothing is happening)\n");
            } else if (hr == 0) {
                printf("Danger (low HR without movement)\n");
            } else {
                printf("Stress (High HR and normal A_G)\n");
            }
            ct++;
        }

        for (i = 0; i < 4; i++) {
            lt[i] = lt[i + 1];
        }
    }
}
 /* Uncomment the following for try it
int main() {
    int sample1[] = {158233, 157389, 156587, 156082, 155676, 155377, 155204, 155121, 155027, 155010,
        155045, 155232, 155480, 155740, 155923, 156111, 156350, 156629, 156938, 157262,
        157655, 158051, 158540, 158915, 158754, 158233, 157839, 157704, 157601, 157495,
        157569, 157578, 157631, 157701, 157865, 158064, 158264, 158511, 158744, 159075,
        159303, 159508, 159941, 160288, 160321, 159943, 159355, 158965, 158773, 158718,
        158710, 158607, 158540, 158370, 158349, 158396, 158434, 158550, 158618, 158766,
        158880, 159042, 159242, 159354, 159392, 159003, 158569, 158328, 158184, 158119,
        158180, 158266, 158280, 158325, 158418, 158574, 158732, 158888, 159129, 159321,
        159515, 159757, 160081, 160378, 160561, 160643, 160308, 159857, 159557, 159434,
        159245, 159283, 159328, 159305, 159355, 159425, 159516, 159702, 159893, 160111,
        160327, 160526, 160802};

    int valhr = HR(sample1, sizeof(sample1) / sizeof(sample1[0]));
    //printf("%d\n", valhr);
    runStressDetectionAlgorithm(5, valhr);
    return 0;
} */
