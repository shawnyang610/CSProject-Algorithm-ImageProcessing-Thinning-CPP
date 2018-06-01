//
//  main.cpp


#include <iostream>
#include <fstream>
using namespace std;

class ThinningSkeleton {
public:
    ifstream *infile;
    ofstream *outfile1;
    ofstream *outfile2;
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    bool changeFlag;
    int CycleCount;
    int ** firstAry;
    int ** secondAry;
    
    class Number_Of_ConnectedNeighbours{
    public:
        class Neighbor{
        public:
            int row;
            int col;
            Neighbor *next;
            Neighbor (){
                row=-1;
                col=-1;
                next=NULL;
            }
            Neighbor (int in_row, int in_col){
                row=in_row;
                col=in_col;
                next=NULL;
            }
        };
        
        static int getCount(int num_nonezero_neighbors, int** in_zeroFramedAry){
            int counter =0;
            Neighbor *currentNeighbor=NULL;
            Neighbor *neighborSet = new Neighbor();
            Neighbor *neighborCandidate = new Neighbor();
            //3 scan the neighborhood, neighborCandidate <- first nonezero neighbor we see
            for (int i=1; i<4; i++){
                for (int j=1; j<4; j++){
                    if (in_zeroFramedAry[i][j]!=0){
                        neighborCandidate->next = new Neighbor(i,j);
                        goto label1;
                    }
                }
            }
        label1:
            while (true){
            label2:
                //4
                if (neighborCandidate->next !=NULL){
                    currentNeighbor = neighborCandidate->next;
                    neighborCandidate->next = neighborCandidate ->next->next;
                }
                else
                    break;
                //5
                bool isExist = false;
                Neighbor *walker = neighborSet;
                while (walker->next !=NULL){
                    if (currentNeighbor->row==walker->next->row && currentNeighbor->col == walker->next ->col){
                        isExist=true;
                        goto label2;
                    }
                    walker = walker->next;
                }
                counter++;
                currentNeighbor->next = neighborSet->next;
                neighborSet->next=currentNeighbor;
                //6 scan all neighbors of currentNeighbor, if p(i,j)>1, push to neighborCandidate
                for (int i=0; i<3; i++){
                    for (int j=0; j<3; j++){
                        if (i==1 && j==1){}
                        else if (in_zeroFramedAry[currentNeighbor->row-1+i][currentNeighbor->col-1+j]>0){
                            Neighbor *newNeighbor = new Neighbor(currentNeighbor->row-1+i, currentNeighbor->col-1+j);
                            newNeighbor->next = neighborCandidate->next;
                            neighborCandidate->next=newNeighbor;
                            //currentNeighbor->next=neighborCandidate->next;
                           // neighborCandidate->next = currentNeighbor;
                        }
                    }
                }
            }
            return counter;
        }
        
    };
    ThinningSkeleton(ifstream *in_infile, ofstream *in_outfile1, ofstream *in_outfile2){
        //step 0, 1
        changeFlag=true;
        infile = in_infile;
        outfile1=in_outfile1;
        outfile2=in_outfile2;
        *infile >> numRows>>numCols>>minVal>>maxVal;
        firstAry = new int *[numRows+2];
        for (int i=0; i<numRows+2; i++){
            firstAry[i]=new int[numCols+2];
            for (int j=0; j<numCols+2; j++){
                firstAry[i][j]=0;
            }
        }
        secondAry = new int*[numRows+2];
        for (int i=0; i<numRows+2; i++){
            secondAry[i]=new int[numCols+2];
            for (int j=0; j<numCols+2; j++){
                secondAry[i][j]=0;
            }
        }
    }
    
    void run(){
        //step 2
        loadImage(secondAry);
        copyAry();
        //step 3
        CycleCount=0;
        while (changeFlag==true){
            //step 4
            if (CycleCount==0 || CycleCount==2 || CycleCount==4){
                prettyPrint();
            }
            //step 5
            changeFlag=false;
            CycleCount++;
            //step 6
            northThinning();
            copyAry();
            //step 7
            southThinning();
            copyAry();
            //step 8
            westThinning();
            copyAry();
            //step 9
            eastThinning();
            copyAry();
        } // step 10 repeat 4-9
        //step 11
        prettyPrint();
        //step 12
        outputToFile();
    }
    
    
    void loadImage(int** in_Ary){
        for (int r=1; r<numRows+1; r++){
            for (int c=1; c<numCols+1; c++){
                *infile >> in_Ary[r][c];
            }
        }
    }
    void copyAry(){
        for (int r=0; r<numRows+2; r++){
            for (int c=0; c< numCols+2; c++){
                firstAry[r][c]=secondAry[r][c];
            }
        }
    }
    void doThinning(int in_row, int in_col){
        //todo
        //find number of nonezero neighbors
        int nonezeroNeighborCount=0;
        for (int i=0; i<3; i++){
            for (int j=0; j<3; j++){
                if (i==1 && j==1){}
                else if (firstAry[in_row-1+i][in_col-1+j]>0)
                        nonezeroNeighborCount++;
            }
        }
        if (nonezeroNeighborCount>=4 && isOnlyOneComponent(in_row, in_col, nonezeroNeighborCount)){
            secondAry[in_row][in_col]=0;
            changeFlag=true;
        }
        else secondAry[in_row][in_col]=1;
    }
    
    bool isOnlyOneComponent (int in_r, int in_c, int in_numOfNoneZeroNeighbors){
        //find how many none zero neighbors in total
        //1.allocate zeroFramedAray for storing neighbors
        int ** zeroFramedAry = new int*[5];
        for (int i=0; i < 5; i++){
            zeroFramedAry[i]=new int[5];
            for (int j=0; j<5; j++){
                zeroFramedAry[i][j]=0;
            }
        }
        //2. zeroFramedAry <- all neighbors w/o P
        for (int i=1; i<4; i++){
            for (int j=1; j<4; j++){
                zeroFramedAry[i][j]=firstAry[in_r-2+i][in_c-2+j];
            }
        }
        zeroFramedAry[2][2]=0; //erase p
        if (in_numOfNoneZeroNeighbors!= Number_Of_ConnectedNeighbours::getCount(in_numOfNoneZeroNeighbors, zeroFramedAry)){
            return false;
        }
        else return true;
    }
    

    void northThinning(){
        //todo
        for (int r=1; r<numRows+1; r++){
            for (int c=1; c<numCols+1; c++){
                if (firstAry[r][c]>0 && firstAry[r-1][c]==0){
                    doThinning(r, c);
                }
            }
        }
    }
    void southThinning(){
        for (int r=1; r<numRows+1; r++){
            for (int c=1; c<numCols+1; c++){
                if (firstAry[r][c]>0 && firstAry[r+1][c]==0){
                    doThinning(r, c);
                }
            }
        }
    }
    void westThinning(){
        for (int r=1; r<numRows+1; r++){
            for (int c=1; c<numCols+1; c++){
                if (firstAry[r][c]>0 && firstAry[r][c-1]==0){
                    doThinning(r, c);
                }
            }
        }
    }
    void eastThinning(){
        for (int r=1; r<numRows+1; r++){
            for (int c=1; c<numCols+1; c++){
                if (firstAry[r][c]>0 && firstAry[r][c+1]==0){
                    doThinning(r, c);
                }
            }
        }
    }
    void prettyPrint(){
        *outfile2 <<"CycleCount: "<<CycleCount<<endl;
        for (int r=1; r<numRows+1; r++){
            for (int c=1; c<numCols+1; c++){
                if (secondAry[r][c]!=0)
                    *outfile2 << "1"<<" ";
                else
                    *outfile2 << "  ";
            }
            *outfile2<<endl;
        }
    }
    void outputToFile(){
        *outfile1 << numRows<<" "<<numCols<<" "<<minVal<<" "<<maxVal<<" "<<endl;
        for (int r=1; r<numRows+1; r++){
            for (int c=1; c<numCols+1; c++){
                *outfile1<<secondAry[r][c]<<" ";
            }
            *outfile1<<endl;
        }
    }
    
};



int main(int argc, const char * argv[]) {
    ifstream infile;
    ofstream outfile1;
    ofstream outfile2;
    
    if (argc<4){
        cout<<"Please supply 1 input 2 output files"<<endl;
        return 1;
    }
    try {
        infile.open(argv[1]);
        outfile1.open(argv[2]);
        outfile2.open(argv[3]);
    } catch(exception &e){
        cout<<"error opening files."<<endl;
        return 1;
    }
    
    ThinningSkeleton *skeleton = new ThinningSkeleton(&infile, &outfile1, &outfile2);
    skeleton->run();
    infile.close();
    outfile1.close();
    outfile2.close();
}
