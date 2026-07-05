/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --expand-div */
typedef struct ExprOpArgs {
    int *op;
    int *dst;
    int *lhs;
    int *rhs;
} ExprOpArgs;

int Math_FixedMul(int lhs, int rhs);
int Math_FixedDivide(int a, int b);

int Task_EvalExpr(ExprOpArgs *args) {
    switch (*args->op) {
    case 0:
        *args->dst = *args->lhs + *args->rhs;
        break;
    case 1:
        *args->dst = *args->lhs - *args->rhs;
        break;
    case 2:
        *args->dst = *args->lhs | *args->rhs;
        break;
    case 3:
        *args->dst = *args->lhs & *args->rhs;
        break;
    case 4:
        *args->dst = *args->lhs ^ *args->rhs;
        break;
    case 5:
        *args->dst = (*args->lhs || *args->rhs);
        break;
    case 6:
        *args->dst = (*args->lhs && *args->rhs);
        break;
    case 7:
        *args->dst = !*args->lhs;
        break;
    case 8:
        *args->dst = ~*args->lhs;
        break;
    case 9:
        *args->dst = *args->lhs > *args->rhs;
        break;
    case 10:
        *args->dst = *args->lhs < *args->rhs;
        break;
    case 11:
        *args->dst = *args->lhs == *args->rhs;
        break;
    case 12:
        *args->dst = *args->lhs >= *args->rhs;
        break;
    case 13:
        *args->dst = *args->lhs <= *args->rhs;
        break;
    case 14:
        *args->dst = *args->lhs != *args->rhs;
        break;
    case 15:
        *args->dst = *args->lhs * *args->rhs;
        break;
    case 16:
        *args->dst = *args->lhs / *args->rhs;
        break;
    case 17:
        *args->dst = *args->lhs << *args->rhs;
        break;
    case 18:
        *args->dst = *args->lhs >> *args->rhs;
        break;
    case 19:
        *args->dst = *args->lhs;
        break;
    case 20:
        *args->dst = Math_FixedMul(*args->lhs, *args->rhs);
        break;
    case 21:
        *args->dst = Math_FixedDivide(*args->lhs, *args->rhs);
        break;
    case 22:
        *args->dst = *args->lhs % *args->rhs;
        break;
    case 23:
        *args->dst = -*args->lhs;
        break;
    }
    return 1;
}
