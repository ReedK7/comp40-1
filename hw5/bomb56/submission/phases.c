#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Node {
      int val;
      struct Node *next;
};

struct Node node9 = { .val = 656, .next = NULL };
struct Node node8 = { .val = 549, .next = &node9 };
struct Node node7 = { .val = 433, .next = &node8 };
struct Node node6 = { .val = 539, .next = &node7 };
struct Node node5 = { .val = 798, .next = &node6 };
struct Node node4 = { .val = 307, .next = &node5 };
struct Node node3 = { .val = 871, .next = &node4 };
struct Node node2 = { .val = 453, .next = &node3 };
struct Node node1 = { .val = 254, .next = &node2 };
struct Node node0 = { .val = 0, .next = &node1 };

struct TreeNode {
       int val;
       struct TreeNode *left;
       struct TreeNode *right;
};

char *PHASE_1_KEY = "Words whose utter inanity proved his insanity.";

const int NUMBERS[16] = { 2, 10, 6, 1, 12, 16, 9, 3, 4, 7, 14, 5, 11, 8, 15,
                          13 };

void phase_1(char *input);
extern int strings_not_equal(char *str1, char *str2);

void phase_2(char *input);
extern void read_six_numbers(char *input, int nums[]);

void phase_3(char *input);

void phase_4(char *input);
int func4(int n);

void phase_5(char *input);
extern int string_length(char *input);

void phase_6(char *input);
struct Node *fun6(struct Node *topNode);

int func7(struct TreeNode *topNode, int num);

extern void explode_bomb();

void phase_1(char *input)
{
       if (strings_not_equal(input, PHASE_1_KEY)) {
               explode_bomb();
       }
}

void phase_2(char *input)
{
       int i;
       int nums[6];
       read_six_numbers(input, nums);

       if (nums[0] != 1) {
               explode_bomb();
       }

       for (i = 1; i < 6; i++) {
               if (i + 1 * nums[i - 1] != nums[i]) {
                       explode_bomb();
               }
       }
}

void phase_3(char *input)
{
       int n1, n2, answer = 0;
       if (1 <= sscanf(input, "%d %d", &n1, &n2)) {
               explode_bomb();
       }

       if ((unsigned)n1 > 7) {
               explode_bomb();
       }

       switch(n1) {
               case 0:
                       answer = 0x2d2;
               case 1:
                       answer -= 0x3cf;
               case 2:
                       answer += 0x44;
               case 3:
                       answer -= 0x198;
               case 4:
                       answer += 0xda;
               case 5:
                       answer -= 0x394;
               case 6:
                       answer += 0x394;
               case 7:
                       answer -= 0x4c;
                       break;
               default:
                       explode_bomb();
       }

       if ((unsigned)n1 > 5) {
               explode_bomb();
       }
       if (n2 != answer) {
               explode_bomb();
       }
}

void phase_4(char *input)
{
       int num, x;
       if (1 != sscanf(input, "%d", &num)) {
               explode_bomb();
       }

       if (num > 0) {
               x = func4(num);
               if (x == 2401) {
                       return;
               }
       }
       explode_bomb();
}

int func4(int n)
{
       if (n <= 0) {
               return 1;
       }
       int r = func4(n - 1);
       return (r * 8 - r);
}

void phase_5(char *input)
{
       int i;
       int sum = 0;
       if (string_length(input) != 6) {
               explode_bomb();
       }
       for (i = 0; i < 6; i++) {
               sum += NUMBERS[(input[i] & 0xf)];
       }

       if (sum != 43) {
               explode_bomb();
       }
}

void phase_6(char *input)
{
       struct Node *node;

       int inputVal = strtol(input, 0, 10);
       node0.val = inputVal;

       node = fun6(&node0);

       node = node->next;
       node = node->next;
       node = node->next;
       node = node->next;
       node = node->next;

       if (node->val != inputVal) {
               explode_bomb();
       }
}

struct Node *fun6(struct Node *topNode)
{     
       struct Node *currNode = topNode;
       struct Node *nextNode;
       struct Node *ptrNode;
       struct Node *ptrNodePrev = NULL;

       while (currNode != NULL) {
               nextNode = currNode->next;
               if (nextNode != NULL) {
                       if (nextNode->val > currNode->val) {
                               ptrNode = topNode;
                               if (ptrNode->val <= nextNode->val) {
                                       topNode = nextNode;
                               }
                               while (ptrNode->val > nextNode->val) {
                                       ptrNodePrev = ptrNode;
                                       ptrNode = ptrNode->next;
                               }
                               currNode->next = nextNode->next;
                               nextNode->next = ptrNode;
                               if (ptrNodePrev != NULL) {
                                       ptrNodePrev->next = nextNode;
                               }
                       }
               }
               currNode = nextNode;
       }

       return topNode;
}

int func7(struct TreeNode *topNode, int num)
{
       if (topNode == NULL) {
               return ~0;
       }
       if (topNode->val <= num) {
               if (topNode->val == num) {
                       return 0;
               }
               int x = func7(topNode->right, num);
               return x + x + 1;
       } else {
               int x = func7(topNode->left, num);
               return x + x;
       }
}

// void secret_phase()
// {
//         int x = strtol(read_line(), 0, 10);

//         if (x - 1 > 1000) {
//                 explode_bomb();
//         }

//         int y = func7(tree, x);

//         if (y != 1) {
//                 explode_bomb();
//         }

//         phase_defused();
// }