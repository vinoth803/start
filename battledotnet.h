/* Node Structure */
struct node {
	char           *playName;
	int          	dotmatrix[10][10];
	pthread_t	thread_id;
	//bool		isBaseDestroyed;	
	struct node* next;
};


/* battleDotNet.c functions declarations */
char * getplayername();
void populate_dotMatrix(int arr[10][10]);
void setDotship(int arr[10][10]);
void display_dotMatrix(int arr[10][10]);
char * include_player(pthread_t tid);
void viewList(); 
int bombPlayer(char *pName, int i, int j); 
char * findopponent(char *pName); 
int remove_player(char *pName); 
void bdn_free(struct node *trace);
bool isNodePresent(char *pName); 
