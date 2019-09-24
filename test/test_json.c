#include "json/json.h"

int main(int argc, char **argv)
{
    id obj;

    json_element_new(&obj);
    json_element_load_file(obj, "inner://res/json/outpu2.json");
    json_element_dump(obj);
    // json_element_save_file(obj, "local://res/json/output2.json");
    release(obj);
}