#include "xml/xml.h"

static void search(id query, id node, const char *s)
{
    id n;
    unsigned index;
    const char *ptr;
    unsigned len;

    debug("\n----------------------------------------\nCOMMAND: %s\n----------------------------------------\n", s);
    xml_query_search(query, node, s);
    
    index = 0;
    xml_query_get_node(query, index, &n);
    while (id_validate(n)) {
        debug("\n");
        xml_node_get_name(n, &ptr, &len);
        debug("node-name: %s\n", ptr);
        xml_node_get_value(n, &ptr, &len);
        debug("node-value: %s\n", ptr);
        index++;
        xml_query_get_node(query, index, &n);
    }

    index = 0;
    xml_query_get_attribute(query, index, &n);
    while(id_validate(n)) {
        debug("\n");
        xml_attribute_get_name(n, &ptr, &len);
        debug("attr-name: %s\n", ptr);
        xml_attribute_get_value(n, &ptr, &len);
        debug("attr-value: %s\n", ptr);

        index++;
        xml_query_get_attribute(query, index, &n);
    }
}

int main(int argc, char **argv)
{
    id node, query;

    xml_node_new(&node);
    xml_node_load_file(node, "inner://res/xml/helloworld.xml");
    xml_query_new(&query);

    search(query, node, "/element");
    search(query, node, "//element");
    search(query, node, "//element[@opt=\"1\"]");
    search(query, node, "//element[@tag=\"tag3\" opt=\"1\"]");
    search(query, node, "./@version");
    search(query, node, "@version");
    search(query, node, "//element/@tag");
    search(query, node, "//@tag");

    release(query);
    release(node);
}