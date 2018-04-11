#include <sympl/core/sympl.h>

int main()
{
    sympl_string_buffer* sb = sympl_string_buffer_create("New string buffer");
    printf("%s\n", sb->buffer);

    sympl_string_buffer_append(sb, ". Added new string to this buffer.");
    printf("%s\n", sb->buffer);

    sympl_string_buffer_clear(sb);
    printf("? - %s\n", sb->buffer);
    return 0;
}
