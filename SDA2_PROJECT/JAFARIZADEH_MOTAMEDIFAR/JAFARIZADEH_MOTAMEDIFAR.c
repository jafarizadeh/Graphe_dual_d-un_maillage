#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"lib/hashmap.h"
#include <time.h> 
#define INF 99999
#define INTMAX 99999
float *Vertices;//mesh vertices
int VertexCount=0;//mesh vertices count
float *DualVertices; //dual graph vertices
int *Facets,FacetCount=0;//facets of the mesh
int *Aretes,AretesCount=0;//edges of the Dual Graph
int *MeshEdges,MeshEdgesCount=0;//edges of the mesh
int MaxDistance=0;//maximum distance between centroids of facets and source node
int counter=0;//a variable to count the dual graph's edges 
int *Distance;//minimum distances between vertices of the generated dual graph and a source vertex
void Read(char *inputFileName){   
    int i=0,j=0;
    FILE * fp;
    char* line=(char*)malloc(100*sizeof(char));
    fp = fopen(inputFileName, "r");
    if(!fp){
     printf("\n Unable to open or create the file: %s ", inputFileName);
     getchar();
     exit(0);
    }
    do{ //count the number of verteces and facets
      strcpy(line,"");
      fgets(line, 100, fp);
      if(*line=='v')
        VertexCount++;
      if(*line=='f')
        FacetCount++;
    }while(*line);
    fseek(fp, 0, SEEK_SET); //return to the begining of input file
    Vertices=(float *)malloc(3 * VertexCount * sizeof(float)); //allocating memory for vertices
    char *token1,*token2,*token3; //define some tokens for parsing the input 
    Facets=(int *)malloc(3 * FacetCount * sizeof(int));  //allocating memory for facets
    MeshEdges=(int *)malloc(2 * 3 * FacetCount * 3 * sizeof(int)); //allocating memory for input mesh edges
    do{
      strcpy(line,"");
      fgets(line, 100, fp);
      if(*line=='v')//parsing the vertices
       {
      token1 =strtok(line," ");
      token1 =strtok(NULL," ");
      *(Vertices + i*3)=atof(token1);
      token2 =strtok(NULL," ");
      *(Vertices + i*3+1)=atof(token2);
      token3 =strtok(NULL," ");
      *(Vertices + i*3+2)=atof(token3);
      i++;
      }
      if(*line=='f'){     //parsing the facets and extraction the mesh edges 
      token1 = strtok(line," ");
      token1 = strtok(NULL," ");
      *(Facets + j*3)=atoi(token1);
      token2 = strtok(NULL," ");
      *(Facets + j*3+1)=atoi(token2);
      token3 = strtok(NULL," ");
      *(Facets + j*3+2)=atoi(token3);
      *(MeshEdges+j*18+0)=atoi(token1);//first edge of the current facet
      *(MeshEdges+j*18+1)=atoi(token2);
      *(MeshEdges+j*18+2)=j+1;
      *(MeshEdges+j*18+3)=atoi(token2);//second edge of the current facet
      *(MeshEdges+j*18+4)=atoi(token3);
      *(MeshEdges+j*18+5)=j+1;
      *(MeshEdges+j*18+6)=atoi(token3);//third edge of the current facet
      *(MeshEdges+j*18+7)=atoi(token1);
      *(MeshEdges+j*18+8)=j+1;
      *(MeshEdges+j*18+9)=atoi(token2);//reverse of the first edge of the current facet
      *(MeshEdges+j*18+10)=atoi(token1);
      *(MeshEdges+j*18+11)=j+1;
      *(MeshEdges+j*18+12)=atoi(token3);//reverse of the second edge of the current facet
      *(MeshEdges+j*18+13)=atoi(token2);
      *(MeshEdges+j*18+14)=j+1;
      *(MeshEdges+j*18+15)=atoi(token1);//reverse of the third edge of the current facet
      *(MeshEdges+j*18+16)=atoi(token3);
      *(MeshEdges+j*18+17)=j+1;
      j++;
      }
    }while(*line);
    MeshEdgesCount=2 * 3 * FacetCount; //Edges of the mesh
    fclose(fp);
}
int areEquvalent(int *FirstArete,int *SecondArete){ //compare two edges of the mesh for equality
  if((*FirstArete==*SecondArete && *(FirstArete+1)==*(SecondArete+1))||(*FirstArete==*(SecondArete+1) && *(FirstArete+1)==*SecondArete))
    return 1;
  else 
    return 0;
}
int estSuperieureA(int *FirstArete,int *SecondArete){//compare two edges of the mesh for dominance
   if(*FirstArete<*SecondArete||(*FirstArete==*SecondArete && *(FirstArete+1)<*(SecondArete+1)))
      return 1;
   else
      return 0;
}
void SelectionSortEdges(void){//selection sort of the edges to find the adjacent facets
    int i,j,MinIndex,temp0,temp1,temp2;
     for (i = 0; i < MeshEdgesCount - 1; i++){ 
        MinIndex = i;
        for (j = i + 1; j < MeshEdgesCount; j++)
            if (estSuperieureA((MeshEdges + 3*j),(MeshEdges+3*MinIndex)))
                MinIndex = j;
        if (MinIndex != i)
            {
            temp0=*(MeshEdges+3*MinIndex);
            temp1=*(MeshEdges+3*MinIndex+1);
            temp2=*(MeshEdges+3*MinIndex+2);
            *(MeshEdges+3*MinIndex)=*(MeshEdges+3*i);
            *(MeshEdges+3*MinIndex+1)= *(MeshEdges+3*i+1);
            *(MeshEdges+3*MinIndex+2)=*(MeshEdges+3*i+2);
            *(MeshEdges+3*i)=temp0;
            *(MeshEdges+3*i+1)=temp1;
            *(MeshEdges+3*i+2)=temp2;
            }
    }
}
//-----------------------------------Heap--------------------------------------------
void HeapCreate(int *MeshEdges, int N, int i)//create a max heap of the edges to find the adjacent facets
{
    int temp0,temp1,temp2;
    int largest = i;
    int left  = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < N && estSuperieureA((MeshEdges + 3*largest),(MeshEdges+3*left)))
        largest = left;
    if (right < N && estSuperieureA((MeshEdges + 3*largest),(MeshEdges+3*right)))
        largest = right;
    if (largest != i) {
            temp0=*(MeshEdges+3*largest);
            temp1=*(MeshEdges+3*largest+1);
            temp2=*(MeshEdges+3*largest+2);
            *(MeshEdges+3*largest)=*(MeshEdges+3*i);
            *(MeshEdges+3*largest+1)= *(MeshEdges+3*i+1);
            *(MeshEdges+3*largest+2)=*(MeshEdges+3*i+2);
            *(MeshEdges+3*i)=temp0;
            *(MeshEdges+3*i+1)=temp1;
            *(MeshEdges+3*i+2)=temp2;
       HeapCreate(MeshEdges, N, largest);
    }
}
void HeapSortEdges(void){//heap sort of the edges to find the adjacent facets
    int i,j,temp0,temp1,temp2;
    for (i = MeshEdgesCount / 2 - 1; i >= 0; i--)
        HeapCreate(MeshEdges, MeshEdgesCount, i);
    for (i = MeshEdgesCount - 1; i >= 0; i--) {
        temp0=*(MeshEdges);
        temp1=*(MeshEdges+1);
        temp2=*(MeshEdges+2);
        *(MeshEdges)=*(MeshEdges+3*i);
        *(MeshEdges+1)= *(MeshEdges+3*i+1);
        *(MeshEdges+2)=*(MeshEdges+3*i+2);
        *(MeshEdges+3*i)=temp0;
        *(MeshEdges+3*i+1)=temp1;
        *(MeshEdges+3*i+2)=temp2;
        HeapCreate(MeshEdges, i, 0);    
    }
}
//------------------------------------AVL--------------------------------------------
struct AVL_Node //each AVL node represents an edge of the mesh
{ 
    int Vertex1,Vertex2,FacetNumber; 
    struct AVL_Node *left; 
    struct AVL_Node *right; 
    int AVL_NodeHeight; 
}; 
struct AVL_Node* new_AVL_Node(int Vertex1,int Vertex2,int FacetNumber) //create a new AVL node which represents an edge of the mesh
{ 
    struct AVL_Node* AVL_node = (struct AVL_Node*)malloc(sizeof(struct AVL_Node)); 
    AVL_node->Vertex1 = Vertex1; 
    AVL_node->Vertex2 = Vertex2; 
    AVL_node->FacetNumber = FacetNumber; 
    AVL_node->left  = NULL; 
    AVL_node->right = NULL; 
    AVL_node->AVL_NodeHeight = 1; 
    return(AVL_node); 
} 
int Height(struct AVL_Node *AVL_node) //a function to find the height of an AVL node 
{ 
    if (AVL_node == NULL) 
        return 0; 
    return AVL_node->AVL_NodeHeight; 
} 
struct AVL_Node *RotateRight(struct AVL_Node *nodeR) //a function for rotating right a subtree on a node to build a balanced tree 
{ 
    struct AVL_Node *nodeA= nodeR->left; 
    struct AVL_Node *nodeA_Right = nodeA->right; 
    nodeA->right = nodeR; 
    nodeR->left = nodeA_Right; 
    if(Height(nodeR->left)<Height(nodeR->right))
        nodeR->AVL_NodeHeight = Height(nodeR->right)+ 1; 
    else
        nodeR->AVL_NodeHeight = Height(nodeR->left) + 1;
    if(Height(nodeA->left)<Height(nodeA->right))
        nodeA->AVL_NodeHeight = Height(nodeA->right)+ 1;
    else
        nodeA->AVL_NodeHeight = Height(nodeA->left) + 1;
    return nodeA; 
} 
struct AVL_Node *RotateLeft(struct AVL_Node *nodeR) //a function for rotating left a subtree on a node to build a balanced tree
{ 
    struct AVL_Node *nodeA = nodeR->right; 
    struct AVL_Node *nodeA_Left = nodeA->left; 
    // Perform rotation 
    nodeA->left = nodeR; 
    nodeR->right = nodeA_Left; 
    //  Update heights 
    if(Height(nodeR->left)<Height(nodeR->right))
        nodeR->AVL_NodeHeight = Height(nodeR->right) + 1;
    else
        nodeR->AVL_NodeHeight = Height(nodeR->left) + 1;
    if(Height(nodeA->left)<Height(nodeA->right))
        nodeA->AVL_NodeHeight = Height(nodeA->right) + 1; 
    else
        nodeA->AVL_NodeHeight = Height(nodeA->left) + 1; 
    // Return new root 
    return nodeA; 
} 
struct AVL_Node* InsertAVL(struct AVL_Node* AVL_node, int Vertex1,int Vertex2, int FacetNumber) //a function to add a node (edge) to the AVL tree
{ 
    int *InsertingEdge=(int *)malloc(3 * sizeof(int));
    *(InsertingEdge)=Vertex1;
    *(InsertingEdge+1)=Vertex2;
    *(InsertingEdge+2)=FacetNumber;
    int *AVL_RootEdge=(int *)malloc(3 * sizeof(int));

    if (AVL_node == NULL) 
      return(new_AVL_Node(Vertex1,Vertex2,FacetNumber)); 
    *(AVL_RootEdge)=AVL_node->Vertex1; //create a node with the current edge
    *(AVL_RootEdge+1)=AVL_node->Vertex2;
    *(AVL_RootEdge+2)=AVL_node->FacetNumber; 
    
    if (estSuperieureA(InsertingEdge,AVL_RootEdge)) // add a node to the AVL tree as a binary search tree
        AVL_node->left  = InsertAVL(AVL_node->left, Vertex1,Vertex2,FacetNumber); 
    else 
        AVL_node->right = InsertAVL(AVL_node->right, Vertex1,Vertex2,FacetNumber); 

    if(Height(AVL_node->left)<Height(AVL_node->right))//updating height of current node
        AVL_node->AVL_NodeHeight = Height(AVL_node->right) + 1; 
    else
        AVL_node->AVL_NodeHeight = Height(AVL_node->left) + 1; 
    
    int BalanceValue;
    if (AVL_node == NULL) //check the new node for its balance
        BalanceValue= 0; 
    else
        BalanceValue= Height(AVL_node->left) - Height(AVL_node->right); 
  
     int *AVL_RootLeftEdge=(int *)malloc(3 * sizeof(int));
     int *AVL_RootRightEdge=(int *)malloc(3 * sizeof(int));

     if(AVL_node->left){//if there is a node in the left child of the current node make an edge with it
     *(AVL_RootLeftEdge)  =AVL_node->left->Vertex1;
     *(AVL_RootLeftEdge+1)=AVL_node->left->Vertex2;
     *(AVL_RootLeftEdge+2)=AVL_node->left->FacetNumber;}

    if(AVL_node->right){ //if there is a node in the right child of the current node make an edge with it
     *(AVL_RootRightEdge)  =AVL_node->right->Vertex1;
     *(AVL_RootRightEdge+1)=AVL_node->right->Vertex2;
     *(AVL_RootRightEdge+2)=AVL_node->right->FacetNumber;}

    // for unbalanced state of the current node, four situations may occur:
    if (BalanceValue > 1  && estSuperieureA(InsertingEdge,AVL_RootLeftEdge)) 
        return RotateRight(AVL_node); 
    if (BalanceValue < -1 && estSuperieureA(AVL_RootRightEdge,InsertingEdge)) 
        return RotateLeft(AVL_node); 
    if (BalanceValue > 1  && estSuperieureA(AVL_RootLeftEdge,InsertingEdge)) 
    { 
        AVL_node->left = RotateLeft(AVL_node->left); 
        return RotateRight(AVL_node); 
    } 
    if (BalanceValue < -1 && estSuperieureA(InsertingEdge,AVL_RootRightEdge)) 
    { 
        AVL_node->right = RotateRight(AVL_node->right); 
        return RotateLeft(AVL_node); 
    } 
    return AVL_node; 
} 
void InorderAVL(struct AVL_Node *AVL_Root) //Inorder traversal of AVL tree to have a sorted list of edges
{ 
    if(AVL_Root != NULL) 
    { 
      InorderAVL(AVL_Root->left);
      *(MeshEdges+counter*3)=AVL_Root->Vertex1;//obtain the solted edge list from the AVL tree
      *(MeshEdges+counter*3+1)=AVL_Root->Vertex2;
      *(MeshEdges+counter*3+2)=AVL_Root->FacetNumber;
      counter++;
      InorderAVL(AVL_Root->right);
    } 
} 
void AVL_SortEdges(void){ //driver function to create and traversal of the AVL tree
    int i;
	struct AVL_Node *AVL_Root = NULL;
    for (i=0; i < MeshEdgesCount; i++)
      AVL_Root=InsertAVL(AVL_Root,*(MeshEdges+i*3),*(MeshEdges+i*3+1),*(MeshEdges+i*3+2));
    counter=0;
    InorderAVL(AVL_Root);   
}
//-------------------------------Hash Table-------------------------------------
char* CreateKey(int Vertex1,int NextIndex)//create an indexed key from first vertex of edge to store it in the hash table
{   char Key[30];
    char TempString[20];
    int i = 0,j=0,k=0,rem;
    while (Vertex1 != 0){//convert intiger value of the first vertex to string
        rem = Vertex1 % 10;
        TempString[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        Vertex1 = Vertex1 / 10;
    }
    for(j=i-1;j>=0;j--)
        Key[i-j-1]=TempString[j];//reverse the obtained string for the vertex
    Key[i++]='_'; //add a separator between vertex number and its index
    if(NextIndex==0)
      Key[i++]='0';
    else
    {  k=0;
      while (NextIndex != 0) {//convert integer value of the index of vertex to string
        rem = NextIndex % 10;
        TempString[k++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        NextIndex = NextIndex / 10;
      }
      for(j=k-1;j>=0;j--)//reverse the obtained string for the index
         Key[i++]=TempString[j];
    }
    Key[i] = '\0'; //mark end of the string
    return strdup(Key); //dublicate the key and return it
}
void HashSortEdges(void){//create a hash table for storing the mesh edges and obtain a sorted set of them
   int i,j,k;
   const unsigned initial_size = MeshEdgesCount; //hash table's initial size
   struct hashmap_s hashmap;//define the hash table
   hashmap_create(initial_size, &hashmap);//create the hash table
   int* Value;//an array to include the current edge's vertices and its related facet
   char* Key1;//a key to put edges in the hash table 
   char* Key2;//a key to get edges from the hash table 
   void* element;
   //-----------------------put in hash table---------------------------
   for (i=0; i < MeshEdgesCount; i++) 
        for(j=0;;j++)//an index for vertex1 to get and create keys for edges with same first vertices
        {
            Key1=NULL;
            Key1=(char *)malloc(30 * sizeof(char));
            strcpy(Key1,CreateKey(*(MeshEdges+i*3),j));//create a key for current edge with its first vertex
            element= hashmap_get(&hashmap, Key1, strlen(Key1));//look for the key before put in the hash table
            if(element==NULL) 
            { 
                Value=NULL;
                Value=(int *)malloc(3 * sizeof(int));
                *(Value)=*(MeshEdges+i*3);//create a value for current edge to put in the hash table with key1
                *(Value+1)=*(MeshEdges+i*3+1);
                *(Value+2)=*(MeshEdges+i*3+2);
                hashmap_put(&hashmap, Key1, strlen(Key1),Value);//put the current key1 and value pair in the hash table 
                break;
            }
        }
    //-----------------------get edge from hash table---------------------------
    int* TempEdge1;//a temporary mesh edge
    int* TempEdge2;
    counter=0;
    for (i=1;i<=VertexCount;i++) //generate keys with vertex numbers to get edges from the hash table
      for(j=0;;j++)//index of keys which are related to multiple edges
      {
        Key1=NULL;
        Key1=(char *)malloc(30 * sizeof(char));
        strcpy(Key1,CreateKey(i,j));
        element = hashmap_get(&hashmap, Key1, strlen(Key1));//get the mesh edges from the hash table with the order of their first vertex
        if(NULL == element)//there no element with key1 in the hash table
           break;
        else 
        {    
            TempEdge1=NULL;
            TempEdge1=(int *)malloc(3 * sizeof(int));
            *(TempEdge1)=*((int*)element);//extract first edge from element of hash table
            *(TempEdge1+1)=*((int*)element+1);
            *(TempEdge1+2)=*((int*)element+2);
            for(k=0;;k++){ // an index for second Key2 
                 Key2=NULL;
                 Key2=(char *)malloc(30 * sizeof(char));
                 strcpy(Key2,CreateKey(*(TempEdge1+1),k));//create second key for vertex2 which is connected to vertex1 of mesh edge 
                 element = hashmap_get(&hashmap, Key2, strlen(Key2));//extract second edge from element of hash table
                 if(NULL == element)//there no element with Key2 in the hash table
                    break;
                 else if(*(TempEdge1)==*((int*)element+1)&&*(TempEdge1+2)!=*((int*)element+2)) //is the first vertex of first edge is equal to the second vertex of extracted edge from the hash table and both edges are related to the same facet
                 {  
                    hashmap_remove(&hashmap, Key1, strlen(Key1));  //remove records with Key1 from hash table  
                    //hashmap_remove(&hashmap, Key2, strlen(Key2));  //remove records with Key2 from hash table 
                    TempEdge2=NULL;
                    TempEdge2=(int *)malloc(3 * sizeof(int));
                    *(TempEdge2)=*(int*)element; //extract the second temporary edge from obtained the hash table record
                    *(TempEdge2+1)=*((int*)element+1);
                    *(TempEdge2+2)=*((int*)element+2);
                    counter++;
                    *(MeshEdges+counter*3)=*(TempEdge1); //initialize an edge of mesh with extracted first temporary edge
                    *(MeshEdges+counter*3+1)=*(TempEdge1+1);
                    *(MeshEdges+counter*3+2)=*(TempEdge1+2);
                    counter++;
                    *(MeshEdges+counter*3)=*(TempEdge2);//initialize another edge of mesh with extracted second temporary edge
                    *(MeshEdges+counter*3+1)=*(TempEdge2+1);
                    *(MeshEdges+counter*3+2)=*(TempEdge2+2); 
                 }
            }    
        }
     }
     hashmap_destroy(&hashmap);
}
//------------------------------------------------------------------------------
void Convert(void){//convert the mesh to dual graph using sorted edges
    int i,j;
    DualVertices=(float *)malloc(3 * FacetCount * sizeof(float));//vertices of dual graph
    Aretes=(int *)malloc(2*MeshEdgesCount*sizeof(int)); //edges of the dual graph 
    for(i=0;i<FacetCount;i++)//convert vertices of mesh to the vertices of dual graph using average of coordinates
       {
        *(DualVertices+i*3)  =(*(Vertices+(*(Facets+i*3)-1)*3)  
                              +*(Vertices+(*(Facets+i*3+1)-1)*3)  
                              +*(Vertices+(*(Facets+i*3+2)-1)*3))/3;   //average of x coordinate;

        *(DualVertices+i*3+1)=(*(Vertices+(*(Facets+i*3)-1)*3+1)
                              +*(Vertices+(*(Facets+i*3+1)-1)*3+1)
                              +*(Vertices+(*(Facets+i*3+2)-1)*3+1))/3; //average of y coordinate;
                              
        *(DualVertices+i*3+2)=(*(Vertices+(*(Facets+i*3)-1)*3+2)
                              +*(Vertices+(*(Facets+i*3+1)-1)*3+2)
                              +*(Vertices+(*(Facets+i*3+2)-1)*3+2))/3; //average of z coordinate;
       }
    AretesCount=0;//count of edges between the dual graph's vertices
    for(i=0;i<MeshEdgesCount;i++)
        if(areEquvalent(MeshEdges+3*i,MeshEdges+3*(i+1)))//if the current edge is equal with its next one in the edges list
        { 
          *(Aretes+2*AretesCount)=*(MeshEdges+3*i+2);//initialize first vertex of current edge in the dual graph
          *(Aretes+2*AretesCount+1)=*(MeshEdges+3*(i+1)+2);//initialize second vertex of current edge in the dual graph
          AretesCount++;
        }
} 
void WriteWithVertexColor(char *OutputFileName)//a function to write the dual graph's specifications to the output file
{
    int i,j;

    float Redcolor,GreenColor,BlueColor;
    FILE * fp;
    fp = fopen(OutputFileName, "w");
    if(!fp)
        {
         printf("\n Unable to open or create the : %s ", OutputFileName);
         getchar();
         exit(0);
         }
    char* str1 = (char*)malloc(100);// a string to fill and write in the file
    for(i=0;i<FacetCount;i++)
       {
        Redcolor=(float)1.0/MaxDistance**(Distance+i);//red color changes between 1 and 0 
        GreenColor=1.0-(float)1.0/MaxDistance**(Distance+i);//green color changes between 1 and 0
        BlueColor=0.0;//blue color is not used for the dual graph
        snprintf(str1, 100,"v %f %f %f %f %f %f", 
        *(DualVertices + i*3),*(DualVertices + i*3 + 1),*(DualVertices + i*3 + 2),Redcolor,GreenColor,BlueColor);//arranging the output string including the dual graph's vertices 
        fputs(str1,fp); //write the output string to output file
        fputs("\n",fp); //go to the next line of output string
       }
    for(i=0;i<AretesCount;i++)
       {
        snprintf(str1, 30,"l %d %d ", *(Aretes + i*2), *(Aretes + i*2 + 1)); //arranging the output string including the dual graph's edges 
        fputs(str1,fp); //write the output string to output file
        fputs("\n",fp);//go to the next line of output string
       }
    fclose(fp);//close the output file
}
int MinDistance(int* IncludeSet,int* Distance)
{
    int MinDis = INTMAX, MinIndex,Ver;//initialize the minimum distance between vertices with a large number
    for (Ver = 0; Ver < FacetCount; Ver++)//for all vertices of the dual graph
        if (*(IncludeSet+Ver) == 0 && *(Distance+Ver) <= MinDis)//if vertex Ver is not in include set and its distance is lower than minimum
            {
                MinDis = *(Distance+Ver);//change the minimum distance
                MinIndex = Ver;//change the minimum vertex index
            }
    return MinIndex;//return the index of vertex minimum that is not in include set and has the minimum distance from source vertex
}
void Dijkstra(int* DualGraph, int SourceVertex,int* Distance) // a function for calculating shortest paths from vertices to the source vertex
{
    int* IncludeSet, i, j; // an array to hold vertices which their shortest path to the source vertex are computed when the ith vertex included, IncludeSet(i)=1 
    IncludeSet=(int*)malloc(FacetCount*sizeof(int)); 
    for (i = 0; i < FacetCount; i++)
        {
            *(Distance+i) = INTMAX; //the distances from source vertex are initiated with infinity 
            *(IncludeSet+i) = 0; //there is no vertex in the include set
        }
    *(Distance+SourceVertex) = 0;//distance of source from itself is 0
    for (i = 0; i < FacetCount - 1; i++)//look for shortest path to source vertex from other vertices of the dualgraph 
    {
        int MinIndex = MinDistance(IncludeSet,Distance);//obtain the vertex with minimum distance to source which is not in the include set
        *(IncludeSet+MinIndex) = 1;//put the selected index with minimum distance in the include set
        for (j = 0; j < FacetCount; j++)//update distance of unselected vertices which are adjacent of the vertex with minimum distance
            //update Distance[j] when j is not in include set and we have an edge from i to j and sum of weights from source to i via j is lower than distance[i]
            if (!*(IncludeSet+j) && *(DualGraph+MinIndex*FacetCount+j) 
                 && *(Distance+MinIndex) != INTMAX && *(Distance+MinIndex) + *(DualGraph+MinIndex*FacetCount+j) < *(Distance+j)) 
                    *(Distance+j) = *(Distance+MinIndex) + *(DualGraph+MinIndex*FacetCount+j);
    }
}
void Coloring(){  // a function to color the generated dualgraph
int i, j;
Distance=(int*)malloc(FacetCount*sizeof(int));
  int* DualGraphAdjacency=(int *)malloc(FacetCount * FacetCount * sizeof(int)); // adjacency matrix for the generated dual graph
  for (i = 0; i < FacetCount; i++) //create adjacency matrix for generated dual graph
     for (j = 0; j < FacetCount; j++) 
        if(i==j)
            *(DualGraphAdjacency+i*FacetCount+j)=0;//distance between each vertex with itself is 0
        else
            *(DualGraphAdjacency+i*FacetCount+j)=INF;
  for(i=0;i<AretesCount;i++)//the edges of the dualgraph is added to the adjacency matrix 
       {
        *(DualGraphAdjacency+(*(Aretes+i*2)-1)*FacetCount+*(Aretes+i*2+1)-1)=1;
        *(DualGraphAdjacency+(*(Aretes+i*2+1)-1)*FacetCount+*(Aretes+i*2)-1)=1;
       }
  Dijkstra(DualGraphAdjacency,1,Distance);//use the dijkstra method to find the shortest paths from all vertices to the source vertex=0
  for (i = 0; i < FacetCount; i++)//calculate the maximum distance between vertices and the source vertex
      if(MaxDistance<*(Distance+i))
          MaxDistance=*(Distance+i);
}
void WriteWithoutVertexColor(char *outputFileName)//a function to write the dual graph's specifications to the output file
{
    int i;
    FILE * fp;
    fp = fopen(outputFileName, "w");
    if(!fp)
        {
         printf("\n Unable to open or create the : %s ", outputFileName);
         getchar();
         exit(0);
         }
    char* str1 = (char*)malloc(100);// a string to fill and write in the file
    for(i=0;i<FacetCount;i++)
       {
        snprintf(str1, 100,"v %f %f %f", 
        *(DualVertices + i*3),*(DualVertices + i*3 + 1),*(DualVertices + i*3 + 2));//arranging the output string including the dual graph's vertices 
        fputs(str1,fp); //write the output string to output file
        fputs("\n",fp); //go to the next line of output string
       }
    for(i=0;i<AretesCount;i++)
       {
        snprintf(str1, 30,"l %d %d ", *(Aretes + i*2), *(Aretes + i*2 + 1)); //arranging the output string including the dual graph's edges 
        fputs(str1,fp); //write the output string to output file
        fputs("\n",fp);//go to the next line of output string
       }
    fclose(fp);//close the output file
}
int main()
{
    char* InputCommand=(char*)malloc(100*sizeof(char));
    char* InputFileName=(char*)malloc(20*sizeof(char));
    char* OutputFileName=(char*)malloc(20*sizeof(char));
    clock_t time;
    char *Token1,*Token2,*Token3,*Token4; //defining 4 tokens for parsing the input   
   	//Enter input command as a string    
   	printf("Enter \"input file name with .obj\"\n\"destination file name with .obj\"\n\"the algorithm name to generate the dual graph\"\n (selectionsort or heapsort or avltree or hashtable)\n and \"y for coloring the dual graph and n for not coloring\" with space for separation:\nExample: bunny10k.obj out_bunny10k.obj avltree y\n");
	gets(InputCommand);
   	Token1 =strtok(InputCommand," ");
   	Token2 =strtok(NULL," ");
   	Token3 =strtok(NULL," ");
 	Token4 =strtok(NULL," ");
   	strcpy(InputFileName,Token1);
  	strcpy(OutputFileName,Token2);
    Read(InputFileName);
   	time=clock();
    if(!strcmp(Token3,"selectionsort"))
       	SelectionSortEdges();
	else if(!strcmp(Token3,"heapsort"))
   		HeapSortEdges();
	else if(!strcmp(Token3,"avltree"))
	   	AVL_SortEdges();
	else if(!strcmp(Token3,"hashtable"))
		HashSortEdges();    	
    Convert();   
    time=clock()-time;         
    if(!strcmp(Token4,"y"))
    {
        Coloring();
        WriteWithVertexColor(OutputFileName);
    }
    else
        WriteWithoutVertexColor(OutputFileName);
    double consumed_time =((double)time)/CLOCKS_PER_SEC;
    printf("\n Consumed time to convert a mesh with %d vertices and %d edges to its dual graph with %s method is:%f seconds",VertexCount,MeshEdgesCount,Token3,consumed_time);
 }
