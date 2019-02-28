#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/**
*主要概念:用一個大小為64的state array把所有state的相關資訊事先記錄下來,要編碼或解碼時再使用
*
*解碼:用2-dimension array儲存trellis diagram中(1)每個state的hamming distance以及
*(2)該狀態的前一個狀態與(3)前一個狀態的input(1 or 0),以上三項資訊用自訂的TrellisState保存
*步驟:
*Step 1: 先計算所有路徑累加的hamming distance,並儲存起來
*Step 2: traceback,利用一開始已經紀錄的前一個state的資訊一路往回走,並記錄此路徑所代表的input
*        而將此路徑反轉即是解碼後的結果
*
*/


typedef struct
{
    char* zero; // input是0時的編碼
    char* one; // input是1時的編碼
    int zeroNextState; // input是1時的next state
    int oneNextState;
} State;

typedef struct
{
    int hammingDis;
    int prevInput;// 1 or 0
    int prevState;
} TrellisState;

State states[64]; // the index is the state No.,儲存state相關資訊,非trellis diagram

TrellisState trellis[415][64];

char* itobstr(int a); // integer轉成二進位字串
char* encode(int stateNum, int in); // get encoded string for every state, input只有0或1
int getNextState(int curState,int in);// get next state

char* encoder(char* inputStr);// 傳回encode之後的字串
char* decoder(char* inputStr);// 傳回decode之後的字串

char* g1 = "1001111";
char* g2 = "1010111";
char* g3 = "1101101";

int main()
{

    int i = 0;
    for(; i < 64 ; i++) //FSM只有64個state,與trellis diagram不同
    {
        states[i].zero = encode(i, 0);
        states[i].one = encode(i, 1);
        states[i].zeroNextState = getNextState(i, 0);
        states[i].oneNextState = getNextState(i, 1);
    }

//initialize the trellis diagram
    i = 0;
    int j = 0;
    for(; i < 415 ; i++)
    {
        j = 0;
        for(; j < 64 ; j++)
        {
            trellis[i][j].hammingDis = -1;
        }
    }

    trellis[0][0].hammingDis = 0;


    FILE *fileInput = fopen("input.txt", "r");
    FILE *fileOutput = fopen("output.txt", "w");

    char *line = (char*)malloc(1243 * sizeof(char));
    line[1242] = '\0';
    fscanf(fileInput, "%s", line);

    char* res = encoder(decoder(line));

    int x = 0;
    for(; x < 1242 ; x++)
    {
        if(line[x] != res[x])
            fprintf(fileOutput, "%d ", x + 1);
    }
    fprintf(fileOutput, "\n%s", res);
    return 0;
}

char* itobstr(int a) // integer轉成二進位字串, 用來得到state number的二進位表示
{
    char* s = (char*) malloc(sizeof(char) * 7);
    int i;
    for(i = 5 ; i >= 0 ; i--)
    {
        s[i] = (char)(a % 2 + 48);
        a /= 2;
    }
    s[6] = '\0';
    return s;
}

char* encode(int stateNum, int in) // 計算每個state的input是0或1時的編碼
{
    // get binary string and append the input
    char* input = (char*)malloc(2 * sizeof(char));
    input[1] = '\0';
    if(in == 0)
        input[0] = '0';
    else
        input[0] = '1';

    char* a = itobstr(stateNum); // 二進位的state No.
    strcat(a, input);
    free(input);
    //printf("%s\n", a);
//reverse
    size_t l = strlen(a);
    char* r = (char*)malloc((l + 1) * sizeof(char));
    r[l] = '\0';
    int i;
    for(i = 0; i < l; i++)
    {
        r[i] = a[l - 1 - i];
    }
    free(a);
    //printf("%s\n", r);
//乘法計算再%2
    char* result = (char*)malloc(4 * sizeof(char));
    result[3] = '\0';
    int index = 0;

    int count = 0;
    int j = 0;
    for(; j < strlen(r) ; j++)
    {
        count += (r[j] - 48) * (g1[j] - 48);
    }
    count = count % 2;
    result[index] = (char)(count + 48);
    index++;

    count = 0;
    j = 0;
    for(; j < strlen(r) ; j++)
    {
        count += (r[j] - 48) * (g2[j] - 48);
    }
    count = count % 2;
    result[index] = (char)(count + 48);
    index++;

    count = 0;
    j = 0;
    for(; j < strlen(r) ; j++)
    {
        count += (r[j] - 48) * (g3[j] - 48);
    }
    count = count % 2;
    result[index] = (char)(count + 48);
    index++;
    free(r);
    //printf("%s\n\n", result);

    return result;
}

int getNextState(int curState, int in)
{
    char* input = (char*)malloc(2 * sizeof(char));
    input[1] = '\0';
    if(in == 0)
        input[0] = '0';
    else
        input[0] = '1';

    char* a = itobstr(curState); // 二進位的state No.
    strcat(a, input);
    free(input);
    a++;
    int next = strtol(a, NULL, 2); // 把a轉換成十進位的integer

    return next;
}

char* encoder(char* inputStr)
{
    char* output = (char*) malloc(sizeof(char) * (( (strlen(inputStr) + 6) * 3 ) + 1));
    output[(strlen(inputStr) + 6) * 3] = '\0';

    int j = 0;
    int currentState = 0;
    for(; j < strlen(inputStr) ; j++)
    {
        if(inputStr[j] == '0')
        {
            strcpy(output + (j*3), states[currentState].zero);
            currentState = states[currentState].zeroNextState;
        }
        else
        {
            strcpy(output + (j*3), states[currentState].one);
            currentState = states[currentState].oneNextState;
        }
    }

    while(currentState != 0)
    {
        strcat(output, states[currentState].zero);
        currentState = states[currentState].zeroNextState;
    }

    return output;
}

char* decoder(char* inputStr)
{
    int segment = strlen(inputStr) / 3; //3個一組

    char* output = (char*) malloc(sizeof(char) * (segment + 1 - 6));
    output[segment - 6] = '\0';

    int hamDis_0 = 0, hamDis_1 = 0;

    int j = 0, k = 0;
    for(; j < segment ; j++) // build the trellis diagram
    {
        k = 0;
        for(; k < 64 ; k++) // k is the current state
        {
            if(trellis[j][k].hammingDis != -1)
            {
                if(inputStr[j * 3] != states[k].zero[0])//calculate the hamming distance
                    hamDis_0++;
                if(inputStr[j * 3 + 1] != states[k].zero[1])
                    hamDis_0++;
                if(inputStr[j * 3 + 2] != states[k].zero[2])
                    hamDis_0++;

                if(trellis[j + 1][states[k].zeroNextState].hammingDis == -1)
                {
                    trellis[j + 1][states[k].zeroNextState].hammingDis = hamDis_0 + trellis[j][k].hammingDis;
                    trellis[j + 1][states[k].zeroNextState].prevInput = 0;
                    trellis[j + 1][states[k].zeroNextState].prevState = k;
                }
                else // 如果有兩條路徑可以到同一個state,要確保該state的hamming distance是較小的值
                {
                    if(hamDis_0 + trellis[j][k].hammingDis < trellis[j + 1][states[k].zeroNextState].hammingDis)
                    {
                        trellis[j + 1][states[k].zeroNextState].hammingDis = hamDis_0 + trellis[j][k].hammingDis;
                        trellis[j + 1][states[k].zeroNextState].prevInput = 0;
                        trellis[j + 1][states[k].zeroNextState].prevState = k;
                    }
                }

                if(j < 408) // 408之後用來使state回到零
                {
                    if(inputStr[j * 3] != states[k].one[0])
                        hamDis_1++;
                    if(inputStr[j * 3 + 1] != states[k].one[1])
                        hamDis_1++;
                    if(inputStr[j * 3 + 2] != states[k].one[2])
                        hamDis_1++;

                    if(trellis[j + 1][states[k].oneNextState].hammingDis == -1)
                    {
                        trellis[j + 1][states[k].oneNextState].hammingDis = hamDis_1 + trellis[j][k].hammingDis;
                        trellis[j + 1][states[k].oneNextState].prevInput = 1;
                        trellis[j + 1][states[k].oneNextState].prevState = k;
                    }
                    else
                    {
                        if(hamDis_1 + trellis[j][k].hammingDis < trellis[j + 1][states[k].oneNextState].hammingDis)
                        {
                            trellis[j + 1][states[k].oneNextState].hammingDis = hamDis_1 + trellis[j][k].hammingDis;
                            trellis[j + 1][states[k].oneNextState].prevInput = 1;
                            trellis[j + 1][states[k].oneNextState].prevState = k;
                        }
                    }
                }

            }

            hamDis_0 = 0;//歸零
            hamDis_1 = 0;

        }
    }
//traceback
    int x = 414;
    int minState = 0;
    while(x > 0)
    {
        if(x <= 408)
            output[x - 1] = (char)trellis[x][minState].prevInput + 48; // 由ASCII,integer 轉 char
        minState = trellis[x][minState].prevState;
        x--;
    }

    return output;
}
