#include "json/json.h"

int main(int argc, char **argv)
{
    id obj;

    json_element_new(&obj);
    json_element_load_file(obj, "inner://res/json/test.json");
    json_element_dump(obj);
    release(obj);
}