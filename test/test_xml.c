#include "xml/xml.h"

int main(int argc, char **argv)
{
    id node, query, n;
    unsigned index;
    const char *ptr;
    unsigned len;

    xml_node_new(&node);
    xml_node_load_file(node, "inner://res/xml/helloworld.xml");

    xml_query_new(&query);
    /* search nodes */
    debug("----------------------------------------\nCOMMAND: /element\n----------------------------------------\n");
    xml_query_search(query, node, "/element");
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

    /* search nodes */
    debug("\n----------------------------------------\nCOMMAND: //element\n----------------------------------------\n");
    xml_query_search(query, node, "//element");
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

    /* get current node attribute */
    debug("\n----------------------------------------\nCOMMAND: ./@version\n----------------------------------------\n");
    xml_query_search(query, node, "./@version");
    xml_query_get_attribute(query, 0, &n);
    if (id_validate(n)) {
        debug("\n");
        xml_attribute_get_name(n, &ptr, &len);
        debug("attr-name: %s\n", ptr);
        xml_attribute_get_value(n, &ptr, &len);
        debug("attr-value: %s\n", ptr);
    }

    /* get current node attribute */
    debug("\n----------------------------------------\nCOMMAND: @version\n----------------------------------------\n");
    xml_query_search(query, node, "@version");
    xml_query_get_attribute(query, 0, &n);
    if (id_validate(n)) {
        debug("\n");
        xml_attribute_get_name(n, &ptr, &len);
        debug("attr-name: %s\n", ptr);
        xml_attribute_get_value(n, &ptr, &len);
        debug("attr-value: %s\n", ptr);
    }

    /* get current node attribute */
    debug("\n----------------------------------------\nCOMMAND: //element/@tag\n----------------------------------------\n");
    xml_query_search(query, node, "//element/@tag");
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

    /* get current node attribute */
    debug("\n----------------------------------------\nCOMMAND: //@tag\n----------------------------------------\n");
    xml_query_search(query, node, "//@tag");
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

    release(query);
    release(node);
}