/**
 *  @file cpp_keywords.cpp
 *
 *  Specifies cpp_keywords color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <cpp_keywords.hpp>

#include <theme.hpp>

cpp_keywords::cpp_keywords(const theme_t & theme) : keywords_t(theme) {

/*

    abstract    continue    for new switch
    assert***   default goto*   package synchronized
    boolean do  if  private this
    break   double  implements  protected   throw
    byte    else    import  public  throws
    case    enum****    instanceof  return  transient
    catch   extends int short   try
    char    final   interface   static  void
    class   finally long    strictfp**  volatile
    const*  float   native  super   while

*/

    color_map = {

        { "do",           theme.keyword_color },
        { "else",         theme.keyword_color },
        { "for",          theme.keyword_color },
        { "if",           theme.keyword_color },
        { "switch",       theme.keyword_color },
        { "while",        theme.keyword_color },

        { "case",         theme.keyword_color },
        { "default",      theme.keyword_color },

        { "try",          theme.keyword_color },
        { "catch",        theme.keyword_color },
        { "finally",      theme.keyword_color },
        { "throw",        theme.keyword_color },

        { "break",        theme.keyword_color },
        { "continue",     theme.keyword_color },
        { "goto",         theme.keyword_color },
        { "return",       theme.keyword_color },

        { "synchronized", theme.keyword_color },
        { "abstract",     theme.keyword_color },

        { "package",      theme.keyword_color },
        { "import",       theme.keyword_color },

        { "assert",       theme.keyword_color },
        { "new",          theme.keyword_color },
        { "instanceof",   theme.keyword_color },

        { "static",       theme.storage_color },
        { "native",       theme.storage_color },
        { "strictfp",     theme.storage_color },
        { "transient",    theme.storage_color },
        { "volatile",     theme.storage_color },

        { "private",      theme.keyword_color },
        { "protected",    theme.keyword_color },
        { "public",       theme.keyword_color },

        { "const",        theme.storage_color },
        { "explicit",     theme.storage_color },

        { "boolean"       theme.type_color },
        { "byte"          theme.type_color },
        { "char",         theme.type_color },
        { "class",        theme.type_color },
        { "double",       theme.type_color },
        { "enum",         theme.type_color },
        { "final",        theme.type_color },
        { "float",        theme.type_color },
        { "int",          theme.type_color },
        { "interface",    theme.type_color },
        { "long",         theme.type_color },
        { "short",        theme.type_color },
        { "void",         theme.type_color },

    };

}
