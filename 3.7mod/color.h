// 30 - 37
#define BLACK "\e[1;30m"
#define RED "\e[1;31m"
#define GREEN "\e[1;32m"
#define YELLOW "\e[1;33m"
#define BLUE "\e[1;34m"
// #define BLUE    "\033[34;0;0m"
// \e[1;34m
#define MAGENTA "\e[1;35m"
#define CYAN "\e[1;36m"
#define WHITE "\e[1;37m"

#define RESET "\e[1;0m"


// int lastUsed;

const char *getColorCode(int t_id)
{
    // if(t_id == lastUsed) {
    //     ++t_id;
    //     t_id %= 8;
    // }  

    switch ((t_id + 1) % 8)
    {
    case 0:
        // lastUsed = 0;
        return BLACK;
        break;
    case 1:
        // lastUsed = 1;
        return RED;
        break;
    case 2:
        // lastUsed = 2;
        return GREEN;
        break;
    case 3:
        // lastUsed = 3;
        return YELLOW;
        break;
    case 4:
        // lastUsed = 4;
        return BLUE;
        break;

    case 5:
        // lastUsed = 5;
        return MAGENTA;
        break;
    case 6:
        // lastUsed = 6;
        return CYAN;
        break;
    case 7:
        // lastUsed = 7;
        return WHITE;
        break;
    default:
        return RESET;
        break;
    }
}
const char *resetColorCode()
{
    return RESET;
}