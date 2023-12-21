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

        switch (hr) {case 0:
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
