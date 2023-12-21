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
        
        //printf("%d\n", strt); // descomentar para pruebas
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
                switch (dpp) {
                    case 0 ... 17:
                        HRs[0] = 1;
                        break;
                    case 18 ... 23:
                        HRs[0] = 2;
                        break;
                    default:
                        HRs[0] = 0;
                        break;
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

int runStressDetectionAlgorithm(int gap, int hr) {
    int lt[5] = {0};
    int ct = 0;
    int i;
    int fval[1] = {0};

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

        switch (hr) {
            case 0:
                if (lt[4] - lt[0] > gap && ct>4) {
                	fval[0] = 2;
                    //printf("case 0 and constant movement \n");
                } else if (lt[4] - lt[0] > gap) {
                	fval[0] = 2;
                    //printf("case 0 and sudent movement \n");
                } else {
                	fval[0] = 2;
                	//printf("Case 0 No mevement");
				} // lo anterior bien podría quitarse y únicamente dejar un valor, ya que todos
				//   los casos en los que el ritmo cardiaco es bajo, hay peligro
                break;
            case 1:
                if (lt[4] - lt[0] > gap && ct>4) {
                	fval[0] = 1;
                    //printf("case 1 and constant movement \n");
                } else if (lt[4] - lt[0] > gap) {
                	fval[0] = 2;
                    //printf("case 1 and sudent movement \n");
                } else {
                	fval[0] = 2;
                	//printf("Case 1 No mevement");
				}
                break;
            case 2:
                if (lt[4] - lt[0] > gap && ct>4) {
                	fval[0] = 2;
                    //printf("case 2 and constant movement: %d \n", fval[0]);
                } else if (lt[4] - lt[0] > gap) {
                	fval[0] = 0;
                    //printf("case 2 and sudent movement: %d \n", fval[0]);
                } else {
                	fval[0] = 0;
                	//printf("Case 2 No movement: %d \n", fval[0]);
				}
                break;
            default:
                printf("Error\n");
                break; // mejorar en futuras versiones, no hace falta este default
        }
		return fval[0];
        ct++;
        for (i = 0; i < 4; i++) {
            lt[i] = lt[i + 1];
        }
    }
}
 
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
    //runStressDetectionAlgorithm(5, valhr); // descomentar para pruebas
    int fv = runStressDetectionAlgorithm(5, valhr);
    printf("%d \n", fv);
    return 0;
}

