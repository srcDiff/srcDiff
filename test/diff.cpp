<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" xmlns:lit="http://www.sdml.info/srcML/literal" xmlns:op="http://www.sdml.info/srcML/operator" xmlns:type="http://www.sdml.info/srcML/modifier" xmlns:diff="http://www.sdml.info/srcDiff" language="C++"><comment type="block">/*
  <diff:old>src2srcdiff.cpp</diff:old><diff:new>src2srcml.cpp</diff:new>

  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/</comment>

<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;stdio.h&gt;</cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;fstream&gt;</cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;string&gt;</cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;vector&gt;</cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file><lit:literal type="string">"../bin/Options.hpp"</lit:literal></cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file><lit:literal type="string">"../bin/srcMLTranslator.hpp"</lit:literal></cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file><lit:literal type="string">"../bin/Language.hpp"</lit:literal></cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file><lit:literal type="string">"shortest_edit_script.h"</lit:literal></cpp:file></cpp:include>

<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;libxml/xmlreader.h&gt;</cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;libxml/xmlwriter.h&gt;</cpp:file></cpp:include>

<cpp:ifdef>#<cpp:directive>ifdef</cpp:directive> <name>__MINGW32__</name></cpp:ifdef>
<diff:new>
</diff:new><cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;string.h&gt;</cpp:file></cpp:include>
<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file>&lt;io.h&gt;</cpp:file></cpp:include>

<function><type><name>char</name> <type:modifier>*</type:modifier></type> <name>strndup</name><parameter_list>(<param><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>s1</name></decl></param>, <param><decl><type><name>size_t</name></type> <name>n</name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>char</name> <type:modifier>*</type:modifier></type> <name>dest</name> =<init> <expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><call><name>malloc</name><argument_list>(<argument><expr><name>n</name> <op:operator>*</op:operator> <call><name>sizeof</name><argument_list>(<argument><expr><name>char</name></expr></argument>)</argument_list></call> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

  <expr_stmt><expr><call><name>memcpy</name><argument_list>(<argument><expr><name>dest</name></expr></argument>, <argument><expr><name>s1</name></expr></argument>, <argument><expr><name>n</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <expr_stmt><expr><name><name>dest</name><index>[<expr><name>n</name></expr>]</index></name> <op:operator>=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</expr_stmt>

  <return>return <expr><name>dest</name></expr>;</return>
}</block></function>
<diff:new>
</diff:new><cpp:endif>#<cpp:directive>endif</cpp:directive></cpp:endif>

<cpp:include>#<cpp:directive>include</cpp:directive> <cpp:file><lit:literal type="string">"xmlrw.h"</lit:literal></cpp:file></cpp:include>

<comment type="line">// macros</comment>
<diff:new><cpp:define>#<cpp:directive>define</cpp:directive> <name>COMMON</name> -1</cpp:define>
</diff:new><cpp:define>#<cpp:directive>define</cpp:directive> <name>SIZEPLUSLITERAL</name>(s) sizeof(s) - 1, BAD_CAST s</cpp:define>
<cpp:define>#<cpp:directive>define</cpp:directive> <name>LITERALPLUSSIZE</name>(s) BAD_CAST s, sizeof(s) - 1</cpp:define>

<diff:old><cpp:define>#<cpp:directive>define</cpp:directive> <name>COMMON</name> -1</cpp:define></diff:old><diff:new><comment type="line">// constant template for temporary file names</comment></diff:new>
<diff:new><decl_stmt><decl><type><name>char</name> <type:modifier>*</type:modifier></type> <name>srcdiff_template</name> =<init> <expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"srcdifftemp.XXXXXX"</lit:literal></expr></init></decl>;</decl_stmt></diff:new>
<diff:new>
</diff:new><decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>XML_DECLARATION_STANDALONE</name> =<init> <expr><lit:literal type="string">"yes"</lit:literal></expr></init></decl>;</decl_stmt>
<decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>XML_VERSION</name> =<init> <expr><lit:literal type="string">"1.0"</lit:literal></expr></init></decl>;</decl_stmt>

<decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>DIFF_PREFIX</name> =<init> <expr><lit:literal type="string">"diff:"</lit:literal></expr></init></decl>;</decl_stmt>
<decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>DIFF_OLD</name> =<init> <expr><lit:literal type="string">"diff:old"</lit:literal></expr></init></decl>;</decl_stmt>
<decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>DIFF_NEW</name> =<init> <expr><lit:literal type="string">"diff:new"</lit:literal></expr></init></decl>;</decl_stmt>
<decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>DIFF_COMMON</name> =<init> <expr><lit:literal type="string">"diff:common"</lit:literal></expr></init></decl>;</decl_stmt>

<decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>output_encoding</name> =<init> <expr><lit:literal type="string">"UTF-8"</lit:literal></expr></init></decl>;</decl_stmt>

<decl_stmt><decl><type><name>const</name> <name>xmlChar</name><type:modifier>*</type:modifier></type> <name>EDIFF_ATTRIBUTE</name> =<init> <expr><name>BAD_CAST</name> <lit:literal type="string">"type"</lit:literal></expr></init></decl>;</decl_stmt>

<decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>EDIFF_BEGIN</name> =<init> <expr><lit:literal type="string">"start"</lit:literal></expr></init></decl>;</decl_stmt>
<decl_stmt><decl><type><name>const</name> <name>char</name><type:modifier>*</type:modifier></type> <name>EDIFF_END</name> =<init> <expr><lit:literal type="string">"end"</lit:literal></expr></init></decl>;</decl_stmt>

<decl_stmt><diff:old><decl><type><name>xmlNodePtr</name></type> <name>diff_common_start</name></decl></diff:old><diff:new><decl><type><name>xmlNs</name></type> <name>diff</name> =<init>  <expr><block>{ <expr><name>NULL</name></expr>, <expr><name>XML_LOCAL_NAMESPACE</name></expr>, <expr><op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"http://www.sdml.info/srcDiff"</lit:literal></expr>, <expr><op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"diff"</lit:literal></expr>, <expr><name>NULL</name></expr>}</block></expr></init></decl></diff:new>;</decl_stmt>
<diff:old><decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>diff_common_end</name></decl>;</decl_stmt></diff:old>
<diff:old><decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>diff_old_start</name></decl>;</decl_stmt>
<decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>diff_old_end</name></decl>;</decl_stmt>
<decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>diff_new_start</name></decl>;</decl_stmt>
<decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>diff_new_end</name></decl>;</decl_stmt>

<comment type="line">// constant template for temporary file names</comment>
<decl_stmt><decl><type><name>char</name> <type:modifier>*</type:modifier></type> <name>srcdiff_template</name> =<init> <expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"srcdifftemp.XXXXXX"</lit:literal></expr></init></decl>;</decl_stmt>

<decl_stmt><decl><type><name>xmlNs</name></type> <name>diff</name> =<init> <expr><block>{ <expr><name>NULL</name></expr>, <expr><name>XML_LOCAL_NAMESPACE</name></expr>, <expr><op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"http://www.sdml.info/srcDiff"</lit:literal></expr>, <expr><op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"diff"</lit:literal></expr>, <expr><name>NULL</name></expr> }</block></expr></init></decl>;</decl_stmt>

</diff:old><comment type="line">// diff accessor function</comment>
<function><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>line_index</name><parameter_list>(<param><decl><type><name>int</name></type> <name>idx</name></decl></param>, <param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type><name>s</name></decl></param>)</parameter_list> <block>{
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>const</name> <name>char</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>&amp;</type:modifier></type> <name>lines</name> =<init> <expr><op:operator>*</op:operator><op:operator>(</op:operator><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>const</name> <name>char</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <op:operator>*</op:operator><op:operator>)</op:operator><name>s</name></expr></init></decl>;</decl_stmt>
  <return>return <expr><name><name>lines</name><index>[<expr><name>idx</name></expr>]</index></name></expr>;</return>
}</block></function>

<comment type="line">// diff comparison function</comment>
<function><type><name>int</name></type> <name>line_compare</name><parameter_list>(<param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>e1</name></decl></param>, <param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>e2</name></decl></param>)</parameter_list> <block>{
  <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>str1</name> =<init> <expr><op:operator>(</op:operator><name>const</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>e1</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>str2</name> =<init> <expr><op:operator>(</op:operator><name>const</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>e2</name></expr></init></decl>;</decl_stmt>

  <return>return <expr><call><name>strcmp</name><argument_list>(<argument><expr><name>str1</name></expr></argument>, <argument><expr><name>str2</name></expr></argument>)</argument_list></call></expr>;</return>
}</block></function>

<comment type="line">// diff node accessor function</comment>
<function><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>node_index</name><parameter_list>(<param><decl><type><name>int</name></type> <name>idx</name></decl></param>, <param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type><name>s</name></decl></param>)</parameter_list> <block>{
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>&amp;</type:modifier></type> <name>nodes</name> =<init> <expr><op:operator>*</op:operator><op:operator>(</op:operator><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <op:operator>*</op:operator><op:operator>)</op:operator><name>s</name></expr></init></decl>;</decl_stmt>
  <return>return <expr><name><name>nodes</name><index>[<expr><name>idx</name></expr>]</index></name></expr>;</return>
}</block></function>

<diff:old><comment type="line">// diff node accessor function</comment>
<function><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>node_set_index</name><parameter_list>(<param><decl><type><name>int</name></type> <name>idx</name></decl></param>, <param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type><name>s</name></decl></param>)</parameter_list> <block>{
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>&amp;</type:modifier></type> <name>node_sets</name> =<init> <expr><op:operator>*</op:operator><op:operator>(</op:operator><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <op:operator>*</op:operator><op:operator>)</op:operator><name>s</name></expr></init></decl>;</decl_stmt>
  <return>return <expr><name><name>node_sets</name><index>[<expr><name>idx</name></expr>]</index></name></expr>;</return>
}</block></function>

</diff:old><comment type="line">// diff node comparison function</comment>
<function><type><name>int</name></type> <name>node_compare</name><parameter_list>(<param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>e1</name></decl></param>, <param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>e2</name></decl></param>)</parameter_list> <diff:old><block>{
  <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>node1</name> =<init> <expr><op:operator>(</op:operator><name>xmlNode</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>e1</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>node2</name> =<init> <expr><op:operator>(</op:operator><name>xmlNode</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>e2</name></expr></init></decl>;</decl_stmt>


  <if>if<condition>(<expr><name>node1</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>node2</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)node1-&gt;name</argument>, <argument>(const char *)node2-&gt;name</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

    <comment type="line">// end if text node contents differ</comment>
    <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node1</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name></expr>)</condition><then>
      <return>return <macro><name>strcmp</name><argument_list>(<argument>(const char *)node1-&gt;content</argument>, <argument>(const char *)node2-&gt;content</argument>)</argument_list></macro>;</return></then>
    <else>else
      <return>return <expr><lit:literal type="number">0</lit:literal></expr>;</return></else></if>
  }</block></then></if>

  <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>
}</block></diff:old><diff:new><block>{
  <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>node1</name> =<init> <expr><op:operator>(</op:operator><name>xmlNode</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>e1</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>node2</name> =<init> <expr><op:operator>(</op:operator><name>xmlNode</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>e2</name></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><name>node1</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>node2</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)node1-&gt;name</argument>, <argument>(const char *)node2-&gt;name</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

    <comment type="line">// end if text node contents differ</comment>
    <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node1</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name></expr>)</condition><then>
      <return>return <macro><name>strcmp</name><argument_list>(<argument>(const char *)node1-&gt;content</argument>, <argument>(const char *)node2-&gt;content</argument>)</argument_list></macro>;</return></then>
    <else>else
      <return>return <expr><lit:literal type="number">0</lit:literal></expr>;</return></else></if>
  }</block></then></if>

  <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>
}</block></diff:new></function>

<diff:old><comment type="line">// diff node comparison function</comment>
<function><type><name>int</name></type> <name>node_set_compare</name><parameter_list>(<param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>e1</name></decl></param>, <param><decl><type><name>const</name> <name>void</name> <type:modifier>*</type:modifier></type> <name>e2</name></decl></param>)</parameter_list> <block>{
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>node_set1</name> =<init> <expr><op:operator>(</op:operator><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <op:operator>*</op:operator><op:operator>)</op:operator><name>e1</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>node_set2</name> =<init> <expr><op:operator>(</op:operator><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <op:operator>*</op:operator><op:operator>)</op:operator><name>e2</name></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><name>node_set1</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <name>node_set2</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>)</condition><then>
    <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return></then>
  <else>else
    <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>node_set1</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
      <if>if<condition>(<expr><call><name>node_compare</name><argument_list>(<argument><expr><name>node_set1</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>node_set2</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>)</condition><then>
        <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return></then></if></for></else></if>

  <return>return <expr><lit:literal type="number">0</lit:literal></expr>;</return>
}</block></function>



</diff:old><comment type="line">// converts source code to srcML</comment>
<function_decl><type><name>void</name></type> <name>translate_to_srcML</name><parameter_list>(<param><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>source_file</name></decl></param>, <param><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>srcml_file</name></decl></param>, <param><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>dir</name></decl></param>)</parameter_list>;</function_decl>

<diff:old><struct>struct <name>tag</name> <block>{<public type="default">

  <decl_stmt><decl><type><name>int</name></type> <name>marked</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>node</name></decl>;</decl_stmt>
</public>}</block>;</struct>

<struct>struct <name>open_diff</name> <block>{<public type="default">

  <decl_stmt><decl><type><name>int</name></type> <name>operation</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>offset</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>tag</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_tags</name></decl>;</decl_stmt>

</public>}</block>;</struct>

</diff:old><comment type="line">// stores information during xml Text Reader processing</comment>
<struct>struct <name>reader_buffer</name> <diff:old><block>{<public type="default">

  <decl_stmt><decl><type><name>int</name></type> <name>stream_source</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>line_number</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>unsigned</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>characters</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>diff_nodes</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diff</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>output_diff</name></decl>;</decl_stmt>

  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>delay_close</name></decl>;</decl_stmt>

</public>}</block></diff:old><diff:new><block>{<public type="default">

  <decl_stmt><decl><type><name>int</name></type> <name>line_number</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>unsigned</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>characters</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>buffer</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>in_diff</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>bool</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>issued_diff</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>context</name></decl>;</decl_stmt>
</public>}</block></diff:new>;</struct>

<comment type="line">// create srcdiff unit</comment>
<function_decl><type><name>xmlNodePtr</name></type> <name>create_srcdiff_unit</name><parameter_list>(<param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name></decl></param>)</parameter_list>;</function_decl>

<comment type="line">// compares a line supposed to be the same and output the correrct elements</comment>
<function_decl><type><name>void</name></type> <name>compare_same_line</name><diff:old><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name></decl></param>,<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list></diff:old><diff:new><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>, <param><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diff</name></decl></param>, <param><decl><type><name>int</name></type> <name>end_line</name></decl></param>)</parameter_list></diff:new>;</function_decl>

<diff:new><function_decl><type><name>int</name></type> <name>merge_same_line</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>, <param><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diff</name></decl></param>, <param><decl><type><name>bool</name> <type:modifier>*</type:modifier></type> <name>mark_open</name></decl></param>)</parameter_list>;</function_decl>

<function_decl><type><name>void</name></type> <name>output_single</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_other</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>, <param><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diff</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>int</name></type> <name>end_line</name></decl></param>)</parameter_list>;</function_decl>

</diff:new><comment type="line">// collect the differnces</comment>
<function_decl><type><name>void</name></type> <name>collect_difference</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>int</name></type> <name>end_line</name></decl></param>)</parameter_list>;</function_decl>

<comment type="line">// output a single difference DELETE or INSERT</comment>
<function_decl><type><name>void</name></type> <name>output_single</name><diff:old><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list></diff:old><diff:new><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list></diff:new>;</function_decl>

<comment type="line">// output a change</comment>
<function_decl><type><name>void</name></type> <name>output_double</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list>;</function_decl>

<function_decl><type><name>void</name></type> <diff:old><name>output_handler</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlNodePtr</name></type> <name>node</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list></diff:old><diff:new><name>update_context</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>)</parameter_list></diff:new>;</function_decl>

<function_decl><diff:old><type><name>bool</name></type></diff:old><diff:new><type><name>void</name></type></diff:new> <diff:old><name>output_peek</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlNodePtr</name></type> <name>node</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list></diff:old><diff:new><name>update_in_diff</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>, <param><decl><type><name>int</name></type> <name>indiff</name></decl></param>)</parameter_list></diff:new>;</function_decl>

<function_decl><type><name>void</name></type> <diff:old><name>update_diff_stack</name><parameter_list>(<param><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diffs</name></decl></param>, <param><decl><type><name>xmlNodePtr</name></type> <name>node</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>)</parameter_list></diff:old><diff:new><name>update_issued_diff</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>)</parameter_list></diff:new>;</function_decl>

<function><type><name>int</name></type> <name>main</name><parameter_list>(<param><decl><type><name>int</name></type> <name>argc</name></decl></param>, <param><decl><type><name>char</name> <type:modifier>*</type:modifier></type> <name><name>argv</name><index>[]</index></name></decl></param>)</parameter_list> <diff:old><block>{

  <comment type="line">// test for correct input</comment>
  <if>if<condition>(<expr><name>argc</name> <op:operator>&lt;</op:operator> <lit:literal type="number">3</lit:literal></expr>)</condition><then> <block>{

    <comment type="line">//    fprintf(stderr, "Usage: src2srcdiff oldFile newFile srcdiffFile\n");</comment>
    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Usage: src2srcdiff oldFile newFile dir\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>
  }</block></then></if>

  <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>srcdiff_file</name></decl>;</decl_stmt>
  <expr_stmt><expr><name>srcdiff_file</name> <op:operator>=</op:operator> <lit:literal type="string">"-"</lit:literal></expr>;</expr_stmt>

  <comment type="block">/*
    std::string * dcommon = new std::string("diff:common");
    std::string * dold = new std::string("diff:old");
    std::string * dnew = new std::string("diff:new");

    diff_common_start = new xmlNode;
    diff_common_start-&gt;name = (xmlChar *)dcommon-&gt;c_str();
    diff_common_start-&gt;type = (xmlElementType)XML_READER_TYPE_ELEMENT;
    diff_common_start-&gt;extra = 0;

    diff_common_end = new xmlNode;
    diff_common_end-&gt;name = (xmlChar *)dcommon-&gt;c_str();
    diff_common_end-&gt;type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
    diff_common_end-&gt;extra = 0;

    diff_old_start = new xmlNode;
    diff_old_start-&gt;name = (xmlChar *)dold-&gt;c_str();
    diff_old_start-&gt;type = (xmlElementType)XML_READER_TYPE_ELEMENT;
    diff_old_start-&gt;extra = 0;

    diff_old_end = new xmlNode;
    diff_old_end-&gt;name = (xmlChar *)dold-&gt;c_str();
    diff_old_end-&gt;type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
    diff_old_end-&gt;extra = 0;

    diff_new_start = new xmlNode;
    diff_new_start-&gt;name = (xmlChar *)dnew-&gt;c_str();
    diff_new_start-&gt;type = (xmlElementType)XML_READER_TYPE_ELEMENT;
    diff_new_start-&gt;extra = 0;

    diff_new_end = new xmlNode;
    diff_new_end-&gt;name = (xmlChar *)dnew-&gt;c_str();
    diff_new_end-&gt;type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
    diff_new_end-&gt;extra = 0;
  */</comment>

  <comment type="block">/*
    Compute the differences between the two source files

  */</comment>

  <comment type="line">// files used for differences</comment>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>char</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></type> <name>lines1</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>char</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></type> <name>lines2</name></decl>;</decl_stmt>

  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name>string</name></name> <type:modifier>*</type:modifier></type> <name>buffer</name> =<init> <expr><op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr></init></decl>;</decl_stmt>

  <comment type="line">// gather file one</comment>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name>ifstream</name></name></type> <name>file1</name></decl>;</decl_stmt>
  <expr_stmt><expr><name>file1</name><op:operator>.</op:operator><call><name>open</name><argument_list>(<argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">1</lit:literal></expr>]</index></name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <expr_stmt><expr><call><name>getline</name><argument_list>(<argument><expr><name>file1</name></expr></argument>, <argument><expr><op:operator>*</op:operator><name>buffer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  <while>while<condition>(<expr><op:operator>!</op:operator><name>file1</name><op:operator>.</op:operator><call><name>eof</name><argument_list>()</argument_list></call></expr>)</condition> <block>{
    <expr_stmt><expr><name>lines1</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>c_str</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>getline</name><argument_list>(<argument><expr><name>file1</name></expr></argument>, <argument><expr><op:operator>*</op:operator><name>buffer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></while>

  <if>if<condition>(<expr><op:operator>*</op:operator><name>buffer</name> <op:operator>!=</op:operator> <lit:literal type="string">""</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>lines1</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>c_str</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>
  <expr_stmt><expr><name>file1</name><op:operator>.</op:operator><call><name>close</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// gather file 2</comment>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name>ifstream</name></name></type> <name>file2</name></decl>;</decl_stmt>
  <expr_stmt><expr><name>file2</name><op:operator>.</op:operator><call><name>open</name><argument_list>(<argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">2</lit:literal></expr>]</index></name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <expr_stmt><expr><call><name>getline</name><argument_list>(<argument><expr><name>file2</name></expr></argument>, <argument><expr><op:operator>*</op:operator><name>buffer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  <while>while<condition>(<expr><op:operator>!</op:operator><name>file2</name><op:operator>.</op:operator><call><name>eof</name><argument_list>()</argument_list></call></expr>)</condition> <block>{
    <expr_stmt><expr><name>lines2</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>c_str</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>getline</name><argument_list>(<argument><expr><name>file2</name></expr></argument>, <argument><expr><op:operator>*</op:operator><name>buffer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></while>

  <if>if<condition>(<expr><op:operator>*</op:operator><name>buffer</name> <op:operator>!=</op:operator> <lit:literal type="string">""</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>lines2</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>c_str</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>
  <expr_stmt><expr><name>file2</name><op:operator>.</op:operator><call><name>close</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// calculate the differences</comment>
  <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit_script</name></decl>;</decl_stmt>

  <decl_stmt><decl><type><name>int</name></type> <name>distance</name> =<init> <expr><call><name>shortest_edit_script</name><argument_list>(<argument><expr><name>lines1</name><op:operator>.</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>, <argument><expr><op:operator>(</op:operator><name>void</name> <op:operator>*</op:operator><op:operator>)</op:operator><op:operator>&amp;</op:operator><name>lines1</name></expr></argument>, <argument><expr><name>lines2</name><op:operator>.</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>, <argument><expr><op:operator>(</op:operator><name>void</name> <op:operator>*</op:operator><op:operator>)</op:operator><op:operator>&amp;</op:operator><name>lines2</name></expr></argument>, <argument><expr><name>line_compare</name></expr></argument>, <argument><expr><name>line_index</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>edit_script</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><name>distance</name> <op:operator>&lt;</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Error with shortest edit script"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <return>return <expr><name>distance</name></expr>;</return>
  }</block></then></if>

  <comment type="block">/*
    Translate both files to srcML separately.
  */</comment>

  <comment type="line">// create temporary file for srcML file one</comment>
  <decl_stmt><decl><type><name>char</name> <type:modifier>*</type:modifier></type> <name>srcml_file_one</name> =<init> <expr><call><name>mktemp</name><argument_list>(<argument><expr><call><name>strdup</name><argument_list>(<argument><expr><name>srcdiff_template</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><name>srcml_file_one</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Tempfile failed\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>
  }</block></then></if>

  <comment type="line">// translate file one</comment>
  <expr_stmt><expr><call><name>translate_to_srcML</name><argument_list>(<argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">1</lit:literal></expr>]</index></name></expr></argument>, <argument><expr><name>srcml_file_one</name></expr></argument>, <argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">3</lit:literal></expr>]</index></name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// create temporary file for srcML file two</comment>
  <decl_stmt><decl><type><name>char</name> <type:modifier>*</type:modifier></type> <name>srcml_file_two</name> =<init> <expr><call><name>mktemp</name><argument_list>(<argument><expr><call><name>strdup</name><argument_list>(<argument><expr><name>srcdiff_template</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><name>srcml_file_two</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Tempfile failed\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>
  }</block></then></if>

  <comment type="line">// translate file two</comment>
  <expr_stmt><expr><call><name>translate_to_srcML</name><argument_list>(<argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">2</lit:literal></expr>]</index></name></expr></argument>, <argument><expr><name>srcml_file_two</name></expr></argument>, <argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">3</lit:literal></expr>]</index></name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <comment type="block">/*
    Create xmlreaders and the xmlwriter
  */</comment>

  <decl_stmt><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name> =<init> <expr><name>NULL</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name> =<init> <expr><name>NULL</name></expr></init></decl>;</decl_stmt>

  <decl_stmt><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name> =<init> <expr><name>NULL</name></expr></init></decl>;</decl_stmt>

  <block>{
    <comment type="line">// create the reader for the old file</comment>
    <expr_stmt><expr><name>reader_old</name> <op:operator>=</op:operator> <call><name>xmlNewTextReaderFilename</name><argument_list>(<argument><expr><name>srcml_file_one</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <if>if <condition>(<expr><name>reader_old</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{
      <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Unable to open file '%s' as XML"</lit:literal></expr></argument>, <argument><expr><name>srcml_file_one</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <goto>goto <name>cleanup</name>;</goto>
    }</block></then></if>

    <comment type="line">// create the reader for the new file</comment>
    <expr_stmt><expr><name>reader_new</name> <op:operator>=</op:operator> <call><name>xmlNewTextReaderFilename</name><argument_list>(<argument><expr><name>srcml_file_two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <if>if <condition>(<expr><name>reader_new</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{
      <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Unable to open file '%s' as XML"</lit:literal></expr></argument>, <argument><expr><name>srcml_file_two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <goto>goto <name>cleanup</name>;</goto>
    }</block></then></if>

    <comment type="line">// create the writer</comment>
    <expr_stmt><expr><name>writer</name> <op:operator>=</op:operator> <call><name>xmlNewTextWriterFilename</name><argument_list>(<argument><expr><name>srcdiff_file</name></expr></argument>, <argument><expr><lit:literal type="number">0</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <if>if <condition>(<expr><name>writer</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{
      <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Unable to open file '%s' as XML"</lit:literal></expr></argument>, <argument><expr><name>srcdiff_file</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <goto>goto <name>cleanup</name>;</goto>
    }</block></then></if>

    <comment type="line">// issue the xml declaration</comment>
    <expr_stmt><expr><call><name>xmlTextWriterStartDocument</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>XML_VERSION</name></expr></argument>, <argument><expr><name>output_encoding</name></expr></argument>, <argument><expr><name>XML_DECLARATION_STANDALONE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></type> <name>output_diff</name></decl>;</decl_stmt>
    <decl_stmt><decl><type>struct <name>open_diff</name> <type:modifier>*</type:modifier></type> <name>new_diff</name> =<init> <expr><op:operator>new</op:operator> struct <name>open_diff</name></expr></init></decl>;</decl_stmt>
    <expr_stmt><expr><name>new_diff</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>=</op:operator> <name>COMMON</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>new_diff</name><op:operator>-&gt;</op:operator><name>open_tags</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>tag</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><name>output_diff</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>new_diff</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <comment type="line">// run through diffs adding markup</comment>
    <decl_stmt><decl><type><name>int</name></type> <name>last_diff</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</decl_stmt>
    <decl_stmt><decl><type>struct <name>reader_buffer</name></type> <name>rbuf_old</name> =<init> <expr><block>{ <expr><name>NULL</name></expr> }</block></expr></init></decl>;</decl_stmt>
    <expr_stmt><expr><name>rbuf_old</name><op:operator>.</op:operator><name>stream_source</name> <op:operator>=</op:operator> <name>DELETE</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_old</name><op:operator>.</op:operator><name>open_diff</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>

    <expr_stmt><expr><name>new_diff</name> <op:operator>=</op:operator> <op:operator>new</op:operator> struct <name>open_diff</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>new_diff</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>=</op:operator> <name>COMMON</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>new_diff</name><op:operator>-&gt;</op:operator><name>open_tags</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>tag</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_old</name><op:operator>.</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>new_diff</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><name>rbuf_old</name><op:operator>.</op:operator><name>output_diff</name> <op:operator>=</op:operator> <op:operator>&amp;</op:operator><name>output_diff</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_old</name><op:operator>.</op:operator><name>delay_close</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <decl_stmt><decl><type>struct <name>reader_buffer</name></type> <name>rbuf_new</name> =<init> <expr><block>{ <expr><name>NULL</name></expr> }</block></expr></init></decl>;</decl_stmt>
    <expr_stmt><expr><name>rbuf_new</name><op:operator>.</op:operator><name>stream_source</name> <op:operator>=</op:operator> <name>INSERT</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_new</name><op:operator>.</op:operator><name>open_diff</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>

    <expr_stmt><expr><name>new_diff</name> <op:operator>=</op:operator> <op:operator>new</op:operator> struct <name>open_diff</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>new_diff</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>=</op:operator> <name>COMMON</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>new_diff</name><op:operator>-&gt;</op:operator><name>open_tags</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>tag</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_new</name><op:operator>.</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>new_diff</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><name>rbuf_new</name><op:operator>.</op:operator><name>output_diff</name> <op:operator>=</op:operator> <op:operator>&amp;</op:operator><name>output_diff</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_new</name><op:operator>.</op:operator><name>delay_close</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <comment type="line">// create srcdiff unit</comment>
    <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>unit</name> =<init> <expr><call><name>create_srcdiff_unit</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

    <comment type="line">// output srcdiff unit</comment>
    <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>unit</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edits</name> =<init> <expr><name>edit_script</name></expr></init></decl>;</decl_stmt>
    <for>for (<init>;</init> <condition><expr><name>edits</name></expr>;</condition> <incr><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr></incr><op:operator>)</op:operator> <block>{

      <comment type="block">/*
      // output diff tag start
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"start\"/&gt;"));
      if(rbuf_old.open_diff-&gt;back()-&gt;operation != COMMON)
      output_handler(&amp;rbuf_old, &amp;rbuf_new, diff_common_start, COMMON, writer);

      rbuf_old.open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
      */</comment>

      <comment type="line">// add preceeding unchanged</comment>
      <if>if<condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>
        <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>.</op:operator><name>line_number</name></expr>, <expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>.</op:operator><name>line_number</name></expr>, <expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
          <expr_stmt><expr><call><name>compare_same_line</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></then>
      <else>else
        <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>.</op:operator><name>line_number</name></expr>, <expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>.</op:operator><name>line_number</name></expr>, <expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
          <expr_stmt><expr><call><name>compare_same_line</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></else></if>

      <comment type="block">/*
        if(rbuf_old.open_diff-&gt;back()-&gt;operation == COMMON &amp;&amp; rbuf_old.open_diff-&gt;size() &gt; 1)
        rbuf_old.open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;

        output_handler(&amp;rbuf_old, &amp;rbuf_new, diff_common_end, COMMON, writer);
      */</comment>

      <comment type="line">// detect and change</comment>
      <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit_next</name> =<init> <expr><name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr></init></decl>;</decl_stmt>
      <if>if<condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name> <op:operator>&amp;&amp;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name> <op:operator>!=</op:operator> <name>NULL</name> <op:operator>&amp;&amp;</op:operator> <name>edit_next</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>INSERT</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator> <op:operator>==</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><call><name>collect_difference</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><call><name>collect_difference</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><call><name>output_double</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
        <expr_stmt><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr>;</expr_stmt>
        <continue>continue;</continue>
      }</block></then></if>

      <comment type="line">// handle pure delete or insert</comment>
      <switch>switch <condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name></expr>)</condition> <block>{

      <case>case <expr><name>INSERT</name></expr>:

        <expr_stmt><expr><call><name>collect_difference</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><call><name>output_single</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>edits</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>
        <break>break;</break>

      </case><case>case <expr><name>DELETE</name></expr>:

        <expr_stmt><expr><call><name>collect_difference</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><call><name>output_single</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>edits</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
        <break>break;</break>
      </case>}</block></switch>

    }</block></for>

    <comment type="block">/*
    // output diff tag start
    //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"start\"/&gt;"));
    if(rbuf_old.open_diff-&gt;back()-&gt;operation != COMMON)
    output_handler(&amp;rbuf_old, &amp;rbuf_new, diff_common_start, COMMON, writer);

    rbuf_old.open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
    */</comment>

    <for>for(<init><decl><type><name>unsigned</name> <name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>lines1</name><op:operator>.</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>.</op:operator><name>line_number</name></expr>, <expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>.</op:operator><name>line_number</name></expr>, <expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
      <expr_stmt><expr><call><name>compare_same_line</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for>

    <comment type="block">/*
      if(rbuf_old.open_diff-&gt;back()-&gt;operation == COMMON &amp;&amp; rbuf_old.open_diff-&gt;size() &gt; 1)
      rbuf_old.open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;

      output_handler(&amp;rbuf_old, &amp;rbuf_new, diff_common_end, COMMON, writer);
    */</comment>
  }</block>

  <comment type="line">// output srcdiff unit</comment>
  <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// cleanup everything</comment>
 <label><name>cleanup</name>:</label>

  <if>if<condition>(<expr><name>reader_old</name></expr>)</condition><then>
    <expr_stmt><expr><call><name>xmlFreeTextReader</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></then></if>

  <if>if<condition>(<expr><name>reader_new</name></expr>)</condition><then>
    <expr_stmt><expr><call><name>xmlFreeTextReader</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></then></if>

  <if>if<condition>(<expr><name>writer</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>xmlTextWriterEndDocument</name><argument_list>(<argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlFreeTextWriter</name><argument_list>(<argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>
  <decl_stmt><decl><type><name>int</name></type> <name>status</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><call><name>remove</name><argument_list>(<argument><expr><name>srcml_file_one</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <op:operator>-</op:operator><lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Remove temp file one failed\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>status</name> <op:operator>=</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>
  }</block></then></if>

  <if>if<condition>(<expr><call><name>remove</name><argument_list>(<argument><expr><name>srcml_file_two</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <op:operator>-</op:operator><lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Remove temp file two failed\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>status</name> <op:operator>=</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>
  }</block></then></if>

  <expr_stmt><expr><call><name>free_shortest_edit_script</name><argument_list>(<argument><expr><name>edit_script</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr><name>status</name></expr>;</return>
}</block></diff:old><diff:new><block>{

  <comment type="line">// test for correct input</comment>
  <if>if<condition>(<expr><name>argc</name> <op:operator>&lt;</op:operator> <lit:literal type="number">3</lit:literal></expr>)</condition><then> <block>{

    <comment type="line">//    fprintf(stderr, "Usage: src2srcdiff oldFile newFile srcdiffFile\n");</comment>
    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Usage: src2srcdiff oldFile newFile dir\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>
  }</block></then></if>

  <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>srcdiff_file</name></decl>;</decl_stmt>
  <expr_stmt><expr><name>srcdiff_file</name> <op:operator>=</op:operator> <lit:literal type="string">"-"</lit:literal></expr>;</expr_stmt>

  <comment type="block">/*
    Compute the differences between the two source files

  */</comment>

  <comment type="line">// files used for differences</comment>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>char</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></type> <name>lines1</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>char</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></type> <name>lines2</name></decl>;</decl_stmt>

  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name>string</name></name> <type:modifier>*</type:modifier></type> <name>buffer</name> =<init> <expr><op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr></init></decl>;</decl_stmt>

  <comment type="line">// gather file one</comment>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name>ifstream</name></name></type> <name>file1</name></decl>;</decl_stmt>
  <expr_stmt><expr><name>file1</name><op:operator>.</op:operator><call><name>open</name><argument_list>(<argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">1</lit:literal></expr>]</index></name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <expr_stmt><expr><call><name>getline</name><argument_list>(<argument><expr><name>file1</name></expr></argument>, <argument><expr><op:operator>*</op:operator><name>buffer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  <while>while<condition>(<expr><op:operator>!</op:operator><name>file1</name><op:operator>.</op:operator><call><name>eof</name><argument_list>()</argument_list></call></expr>)</condition> <block>{
    <expr_stmt><expr><name>lines1</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>c_str</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>getline</name><argument_list>(<argument><expr><name>file1</name></expr></argument>, <argument><expr><op:operator>*</op:operator><name>buffer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></while>

  <if>if<condition>(<expr><op:operator>*</op:operator><name>buffer</name> <op:operator>!=</op:operator> <lit:literal type="string">""</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>lines1</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>c_str</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>
  <expr_stmt><expr><name>file1</name><op:operator>.</op:operator><call><name>close</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// gather file 2</comment>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name>ifstream</name></name></type> <name>file2</name></decl>;</decl_stmt>
  <expr_stmt><expr><name>file2</name><op:operator>.</op:operator><call><name>open</name><argument_list>(<argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">2</lit:literal></expr>]</index></name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <expr_stmt><expr><call><name>getline</name><argument_list>(<argument><expr><name>file2</name></expr></argument>, <argument><expr><op:operator>*</op:operator><name>buffer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  <while>while<condition>(<expr><op:operator>!</op:operator><name>file2</name><op:operator>.</op:operator><call><name>eof</name><argument_list>()</argument_list></call></expr>)</condition> <block>{
    <expr_stmt><expr><name>lines2</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>c_str</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>getline</name><argument_list>(<argument><expr><name>file2</name></expr></argument>, <argument><expr><op:operator>*</op:operator><name>buffer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></while>

  <if>if<condition>(<expr><op:operator>*</op:operator><name>buffer</name> <op:operator>!=</op:operator> <lit:literal type="string">""</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>lines2</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><op:operator>(</op:operator><name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>c_str</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <call><name><name>std</name><op:operator>::</op:operator><name>string</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>
  <expr_stmt><expr><name>file2</name><op:operator>.</op:operator><call><name>close</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// calculate the differences</comment>
  <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit_script</name></decl>;</decl_stmt>

  <decl_stmt><decl><type><name>int</name></type> <name>distance</name> =<init> <expr><call><name>shortest_edit_script</name><argument_list>(<argument><expr><name>lines1</name><op:operator>.</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>, <argument><expr><op:operator>(</op:operator><name>void</name> <op:operator>*</op:operator><op:operator>)</op:operator><op:operator>&amp;</op:operator><name>lines1</name></expr></argument>, <argument><expr><name>lines2</name><op:operator>.</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>, <argument><expr><op:operator>(</op:operator><name>void</name> <op:operator>*</op:operator><op:operator>)</op:operator><op:operator>&amp;</op:operator><name>lines2</name></expr></argument>, <argument><expr><name>line_compare</name></expr></argument>, <argument><expr><name>line_index</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>edit_script</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><name>distance</name> <op:operator>&lt;</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Error with shortest edit script"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <return>return <expr><name>distance</name></expr>;</return>
  }</block></then></if>

  <comment type="block">/*
    Translate both files to srcML separately.
  */</comment>

  <comment type="line">// create temporary file for srcML file one</comment>
  <decl_stmt><decl><type><name>char</name> <type:modifier>*</type:modifier></type> <name>srcml_file_one</name> =<init> <expr><call><name>mktemp</name><argument_list>(<argument><expr><call><name>strdup</name><argument_list>(<argument><expr><name>srcdiff_template</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><name>srcml_file_one</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Tempfile failed\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>
  }</block></then></if>

  <comment type="line">// translate file one</comment>
  <expr_stmt><expr><call><name>translate_to_srcML</name><argument_list>(<argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">1</lit:literal></expr>]</index></name></expr></argument>, <argument><expr><name>srcml_file_one</name></expr></argument>, <argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">3</lit:literal></expr>]</index></name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// create temporary file for srcML file two</comment>
  <decl_stmt><decl><type><name>char</name> <type:modifier>*</type:modifier></type> <name>srcml_file_two</name> =<init> <expr><call><name>mktemp</name><argument_list>(<argument><expr><call><name>strdup</name><argument_list>(<argument><expr><name>srcdiff_template</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><name>srcml_file_two</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Tempfile failed\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>
  }</block></then></if>

  <comment type="line">// translate file two</comment>
  <expr_stmt><expr><call><name>translate_to_srcML</name><argument_list>(<argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">2</lit:literal></expr>]</index></name></expr></argument>, <argument><expr><name>srcml_file_two</name></expr></argument>, <argument><expr><name><name>argv</name><index>[<expr><lit:literal type="number">3</lit:literal></expr>]</index></name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <comment type="block">/*
    Create xmlreaders and the xmlwriter
  */</comment>

  <decl_stmt><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name> =<init> <expr><name>NULL</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name> =<init> <expr><name>NULL</name></expr></init></decl>;</decl_stmt>

  <decl_stmt><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name> =<init> <expr><name>NULL</name></expr></init></decl>;</decl_stmt>

  <block>{
    <comment type="line">// create the reader for the old file</comment>
    <expr_stmt><expr><name>reader_old</name> <op:operator>=</op:operator> <call><name>xmlNewTextReaderFilename</name><argument_list>(<argument><expr><name>srcml_file_one</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <if>if <condition>(<expr><name>reader_old</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{
      <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Unable to open file '%s' as XML"</lit:literal></expr></argument>, <argument><expr><name>srcml_file_one</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <goto>goto <name>cleanup</name>;</goto>
    }</block></then></if>

    <comment type="line">// create the reader for the new file</comment>
    <expr_stmt><expr><name>reader_new</name> <op:operator>=</op:operator> <call><name>xmlNewTextReaderFilename</name><argument_list>(<argument><expr><name>srcml_file_two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <if>if <condition>(<expr><name>reader_new</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{
      <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Unable to open file '%s' as XML"</lit:literal></expr></argument>, <argument><expr><name>srcml_file_two</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <goto>goto <name>cleanup</name>;</goto>
    }</block></then></if>

    <comment type="line">// create the writer</comment>
    <expr_stmt><expr><name>writer</name> <op:operator>=</op:operator> <call><name>xmlNewTextWriterFilename</name><argument_list>(<argument><expr><name>srcdiff_file</name></expr></argument>, <argument><expr><lit:literal type="number">0</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <if>if <condition>(<expr><name>writer</name> <op:operator>==</op:operator> <name>NULL</name></expr>)</condition><then> <block>{
      <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Unable to open file '%s' as XML"</lit:literal></expr></argument>, <argument><expr><name>srcdiff_file</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <goto>goto <name>cleanup</name>;</goto>
    }</block></then></if>

    <comment type="line">// issue the xml declaration</comment>
    <expr_stmt><expr><call><name>xmlTextWriterStartDocument</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>XML_VERSION</name></expr></argument>, <argument><expr><name>output_encoding</name></expr></argument>, <argument><expr><name>XML_DECLARATION_STANDALONE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name></type> <name>open_diff</name></decl>;</decl_stmt>

    <comment type="line">// run through diffs adding markup</comment>
    <decl_stmt><decl><type><name>int</name></type> <name>last_diff</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</decl_stmt>
    <decl_stmt><decl><type>struct <name>reader_buffer</name></type> <name>rbuf_old</name> =<init> <expr><block>{ <expr><name>NULL</name></expr> }</block></expr></init></decl>;</decl_stmt>
    <expr_stmt><expr><name>rbuf_old</name><op:operator>.</op:operator><name>context</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_old</name><op:operator>.</op:operator><name>in_diff</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_old</name><op:operator>.</op:operator><name>issued_diff</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>bool</name></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <decl_stmt><decl><type>struct <name>reader_buffer</name></type> <name>rbuf_new</name> =<init> <expr><block>{ <expr><name>NULL</name></expr> }</block></expr></init></decl>;</decl_stmt>
    <expr_stmt><expr><name>rbuf_new</name><op:operator>.</op:operator><name>context</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_new</name><op:operator>.</op:operator><name>in_diff</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><name>rbuf_new</name><op:operator>.</op:operator><name>issued_diff</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>bool</name></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <comment type="line">// create srcdiff unit</comment>
    <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>unit</name> =<init> <expr><call><name>create_srcdiff_unit</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

    <comment type="line">// output srcdiff unit</comment>
    <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><name>unit</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><name>open_diff</name><op:operator>.</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edits</name> =<init> <expr><name>edit_script</name></expr></init></decl>;</decl_stmt>
    <for>for (<init>;</init> <condition><expr><name>edits</name></expr>;</condition> <incr><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr></incr><op:operator>)</op:operator> <block>{

      <comment type="line">// add preceeding unchanged</comment>
      <if>if<condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>
        <expr_stmt><expr><call><name>compare_same_line</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
      <else>else
        <expr_stmt><expr><call><name>compare_same_line</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if>


      <comment type="line">// detect and change</comment>
      <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit_next</name> =<init> <expr><name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr></init></decl>;</decl_stmt>
      <if>if<condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name> <op:operator>&amp;&amp;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name> <op:operator>!=</op:operator> <name>NULL</name> <op:operator>&amp;&amp;</op:operator> <name>edit_next</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>INSERT</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator> <op:operator>==</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>)</condition><then> <block>{

        <if>if<condition>(<expr><name>open_diff</name><op:operator>.</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <name>INSERT</name></expr>)</condition><then> <block>{

          <expr_stmt><expr><call><name>output_single</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
          <expr_stmt><expr><call><name>output_single</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        }</block></then> <else>else <block>{

          <expr_stmt><expr><call><name>output_single</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
          <expr_stmt><expr><call><name>output_single</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        }</block></else></if>

        <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
        <expr_stmt><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr>;</expr_stmt>
        <continue>continue;</continue>
      }</block></then></if>

      <comment type="line">// handle pure delete or insert</comment>
      <switch>switch <condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name></expr>)</condition> <block>{

      <case>case <expr><name>INSERT</name></expr>:

        <expr_stmt><expr><call><name>output_single</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>
        <break>break;</break>

      </case><case>case <expr><name>DELETE</name></expr>:

        <expr_stmt><expr><call><name>output_single</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
        <break>break;</break>
      </case>}</block></switch>

    }</block></for>

    <expr_stmt><expr><call><name>compare_same_line</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>lines1</name><op:operator>.</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  }</block>

  <comment type="line">// output srcdiff unit</comment>
  <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// cleanup everything</comment>
 <label><name>cleanup</name>:</label>

  <if>if<condition>(<expr><name>reader_old</name></expr>)</condition><then>
    <expr_stmt><expr><call><name>xmlFreeTextReader</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></then></if>

  <if>if<condition>(<expr><name>reader_new</name></expr>)</condition><then>
    <expr_stmt><expr><call><name>xmlFreeTextReader</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></then></if>

  <if>if<condition>(<expr><name>writer</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>xmlTextWriterEndDocument</name><argument_list>(<argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlFreeTextWriter</name><argument_list>(<argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>
  <decl_stmt><decl><type><name>int</name></type> <name>status</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><call><name>remove</name><argument_list>(<argument><expr><name>srcml_file_one</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <op:operator>-</op:operator><lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Remove temp file one failed\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>status</name> <op:operator>=</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>
  }</block></then></if>

  <if>if<condition>(<expr><call><name>remove</name><argument_list>(<argument><expr><name>srcml_file_two</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <op:operator>-</op:operator><lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Remove temp file two failed\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>status</name> <op:operator>=</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>
  }</block></then></if>

  <expr_stmt><expr><call><name>free_shortest_edit_script</name><argument_list>(<argument><expr><name>edit_script</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr><name>status</name></expr>;</return>
}</block></diff:new></function>

<comment type="line">// converts source code to srcML</comment>
<function><type><name>void</name></type> <name>translate_to_srcML</name><parameter_list>(<param><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>source_file</name></decl></param>, <param><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>srcml_file</name></decl></param>, <param><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>dir</name></decl></param>)</parameter_list> <block>{

  <comment type="line">// register default language extensions</comment>
  <expr_stmt><expr><call><name><name>Language</name><op:operator>::</op:operator><name>register_standard_file_extensions</name></name><argument_list>()</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// get language from file extension</comment>
  <decl_stmt><decl><type><name>int</name></type> <name>language</name> =<init> <expr><call><name><name>Language</name><op:operator>::</op:operator><name>getLanguageFromFilename</name></name><argument_list>(<argument><expr><name>source_file</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

  <comment type="line">// select basic options</comment>
  <decl_stmt><decl><type><name>OPTION_TYPE</name></type> <name>options</name> =<init> <expr><name>OPTION_CPP_MARKUP_ELSE</name> <op:operator>|</op:operator> <name>OPTION_CPP</name> <op:operator>|</op:operator> <name>OPTION_XMLDECL</name> <op:operator>|</op:operator> <name>OPTION_XML</name>  <op:operator>|</op:operator> <name>OPTION_LITERAL</name> <op:operator>|</op:operator> <name>OPTION_OPERATOR</name> <op:operator>|</op:operator> <name>OPTION_MODIFIER</name></expr></init></decl>;</decl_stmt>

  <comment type="line">// create translator object</comment>
  <function_decl><type><name>srcMLTranslator</name></type> <name>translator</name><parameter_list>(<param><decl><type><name>language</name></type></decl></param>, <param><decl><type><name>srcml_file</name></type></decl></param>, <param><decl><type><name>options</name></type></decl></param>)</parameter_list>;</function_decl>

  <comment type="line">// set input file (must be done)</comment>
  <expr_stmt><expr><name>translator</name><op:operator>.</op:operator><call><name>setInput</name><argument_list>(<argument><expr><name>source_file</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// translate file</comment>
  <expr_stmt><expr><name>translator</name><op:operator>.</op:operator><call><name>translate</name><argument_list>(<argument><expr><name>NULL</name></expr></argument>, <argument><expr><name>dir</name></expr></argument>, <argument><expr><name>NULL</name></expr></argument>, <argument><expr><name>NULL</name></expr></argument>, <argument><expr><name>language</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <comment type="line">// close the input file</comment>
  <expr_stmt><expr><name>translator</name><op:operator>.</op:operator><call><name>close</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
}</block></function>

<comment type="line">// compares a line supposed to be the same and output the <diff:old>correrct</diff:old><diff:new>correct</diff:new> elements</comment>
<function><type><name>void</name></type> <name>compare_same_line</name><diff:old><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name></decl></param>,<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list></diff:old><diff:new><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>, <param><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diff</name></decl></param>, <param><decl><type><name>int</name></type> <name>end_line</name></decl></param>)</parameter_list></diff:new> <diff:old><block>{

  <decl_stmt><decl><type><name>int</name></type> <name>not_done</name> =<init> <expr><lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
  <while>while<condition>(<expr><name>not_done</name></expr>)</condition> <block>{

    <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)getRealCurrentNode(reader_old)-&gt;name</argument>, <argument>(const char *)getRealCurrentNode(reader_new)-&gt;name</argument>)</argument_list></macro> <expr><op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

      <comment type="line">//fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old-&gt;line_number);</comment>
      <comment type="line">//fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf_new-&gt;line_number);</comment>

      <expr_stmt><expr><call><name>collect_difference</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>line_number</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>collect_difference</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>line_number</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>output_double</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><op:operator>--</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>
      <expr_stmt><expr><op:operator>--</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

      <return>return;</return>

    }</block></then></if>

    <comment type="line">// look if in text node</comment>
    <if>if<condition>(<expr><call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_SIGNIFICANT_WHITESPACE</name> <op:operator>||</op:operator> <call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name></expr>)</condition><then> <block>{

      <comment type="line">// allocate character buffer if empty</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name></expr>)</condition><then> <block>{
        <expr_stmt><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>unsigned</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><call><name>xmlTextReaderConstValue</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>unsigned</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><call><name>xmlTextReaderConstValue</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></then></if>

      <if>if<condition>(<macro><name>strlen</name><argument_list>(<argument>(const char *)rbuf_old-&gt;characters</argument>)</argument_list></macro> <expr><op:operator>!=</op:operator> <macro><name>strlen</name><argument_list>(<argument>(const char *)rbuf_new-&gt;characters</argument>)</argument_list></macro></expr>)</condition><then> <block>{

        <expr_stmt><expr><call><name>collect_difference</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>line_number</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><call><name>collect_difference</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>line_number</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><call><name>output_double</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><op:operator>--</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>
        <expr_stmt><expr><op:operator>--</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

        <return>return;</return>

      }</block></then></if>

      <comment type="line">// cycle through characters</comment>
      <for>for (<init>;</init> <condition><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name></expr>, <expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name></expr></incr><op:operator>)</op:operator> <block>{

        <comment type="line">// escape characters or print out character</comment>
        <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&amp;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;amp;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&lt;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;lt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&gt;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name></expr></argument>, <argument><expr><lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if></else></if></else></if>

        <comment type="line">// increase new line count and exit</comment>
        <if>if<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>==</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition><then> <block>{

          <expr_stmt><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>
          <expr_stmt><expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

          <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

            <expr_stmt><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
            <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          }</block></then></if>

          <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

            <expr_stmt><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
            <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          }</block></then></if>

          <return>return;</return>
        }</block></then></if>
      }</block></for>

      <comment type="line">// end text node if finished and get next node</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
        <expr_stmt><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>

        <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></then></if>
    }</block></then>
    <else>else <block>{

      <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>node</name> =<init> <expr><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

      <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)node-&gt;name</argument>, <argument>"unit"</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <return>return;</return></then></if>

      <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <comment type="line">// output non-text node and get next node</comment>
      <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    }</block></else></if>
  }</block></while>
}</block></diff:old><diff:new><block>{

  <if>if<condition>(<expr><name>end_line</name> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
    <return>return;</return></then></if>

  <decl_stmt><decl><type><name>unsigned</name> <name>int</name></type> <name>last_open_old</name></decl>;</decl_stmt>
  <for>for(<init><expr><name>last_open_old</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr>;</init> <condition><expr><name>last_open_old</name> <op:operator>&gt;</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>)</op:operator><index>[<expr><name>last_open_old</name></expr>]</index> <op:operator>==</op:operator> <name>COMMON</name></expr>;</condition> <incr><expr><op:operator>--</op:operator><name>last_open_old</name></expr></incr><op:operator>)</op:operator><empty_stmt>;</empty_stmt></for>

  <expr_stmt><expr><op:operator>++</op:operator><name>last_open_old</name></expr>;</expr_stmt>

  <decl_stmt><decl><type><name>unsigned</name> <name>int</name></type> <name>last_open_new</name></decl>;</decl_stmt>
  <for>for(<init><expr><name>last_open_new</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr>;</init> <condition><expr><name>last_open_new</name> <op:operator>&gt;</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>)</op:operator><index>[<expr><name>last_open_new</name></expr>]</index> <op:operator>==</op:operator> <name>COMMON</name></expr>;</condition> <incr><expr><op:operator>--</op:operator><name>last_open_new</name></expr></incr><op:operator>)</op:operator><empty_stmt>;</empty_stmt></for>

  <expr_stmt><expr><op:operator>++</op:operator><name>last_open_new</name></expr>;</expr_stmt>

  <decl_stmt><decl><type><name>bool</name></type> <name>mark_open</name> =<init> <expr><lit:literal type="boolean">false</lit:literal></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><op:operator>(</op:operator><name>last_open_old</name> <op:operator>&gt;</op:operator> <lit:literal type="number">1</lit:literal> <op:operator>&amp;&amp;</op:operator> <name>last_open_old</name> <op:operator>==</op:operator> <name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call><op:operator>)</op:operator>
     <op:operator>||</op:operator> <op:operator>(</op:operator><name>last_open_new</name> <op:operator>&gt;</op:operator> <lit:literal type="number">1</lit:literal> <op:operator>&amp;&amp;</op:operator> <name>last_open_new</name> <op:operator>==</op:operator> <name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call><op:operator>)</op:operator></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>

  <decl_stmt><decl><type><name>int</name></type> <name>not_done</name> =<init> <expr><lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>output_type</name> =<init> <expr><name>COMMON</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>output_end</name> =<init> <expr><op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr></init></decl>;</decl_stmt>
  <while>while<condition>(<expr><name>not_done</name></expr>)</condition> <block>{

    <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)getRealCurrentNode(reader_old)-&gt;name</argument>, <argument>(const char *)getRealCurrentNode(reader_new)-&gt;name</argument>)</argument_list></macro> <expr><op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{
 
      <if>if<condition>(<expr><call><name>merge_same_line</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>, <argument><expr><name>open_diff</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>mark_open</name></expr></argument>)</argument_list></call></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr>;</expr_stmt>
      }</block></then></if>

    }</block></then></if>

    <comment type="line">// look if in text node</comment>
    <if>if<condition>(<expr><call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_SIGNIFICANT_WHITESPACE</name> <op:operator>||</op:operator> <call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name></expr>)</condition><then> <block>{

      <comment type="line">// allocate character buffer if empty</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name></expr>)</condition><then> <block>{
        <expr_stmt><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>unsigned</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><call><name>xmlTextReaderConstValue</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>unsigned</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><call><name>xmlTextReaderConstValue</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></then></if>

      <comment type="line">// cycle through characters</comment>
      <for>for (<init>;</init> <condition><expr><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name></expr>, <expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name></expr></incr><op:operator>)</op:operator> <block>{

        <comment type="line">// escape characters or print out character</comment>
        <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&amp;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;amp;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&lt;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;lt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&gt;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name></expr></argument>, <argument><expr><lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if></else></if></else></if>

        <comment type="line">// increase new line count and check if done</comment>
        <if>if<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>==</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition><then> <block>{

          <expr_stmt><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>
          <expr_stmt><expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

          <if>if<condition>(<expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>line_number</name> <op:operator>==</op:operator> <name>end_line</name></expr>)</condition><then> <block>{

            <expr_stmt><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>
            <expr_stmt><expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

            <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

              <expr_stmt><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
              <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            }</block></then></if>

            <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

              <expr_stmt><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
              <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            }</block></then></if>

            <if>if<condition>(<expr><name>mark_open</name></expr>)</condition><then>
              <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then></if>

            <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

            <return>return;</return>
          }</block></then></if>

        }</block></then></if>

      }</block></for>

      <comment type="line">// end text node if finished and get next node</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
        <expr_stmt><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>

        <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></then></if>
    }</block></then>
    <else>else <block>{

      <if>if<condition>(<expr><op:operator>(</op:operator><name>output_type</name> <op:operator>==</op:operator> <name>COMMON</name> <op:operator>||</op:operator> <name>output_type</name> <op:operator>==</op:operator> <name>DELETE</name><op:operator>)</op:operator> <op:operator>&amp;&amp;</op:operator> <name>output_end</name> <op:operator>==</op:operator> <op:operator>(</op:operator><name>signed</name><op:operator>)</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal>
         <op:operator>||</op:operator> <op:operator>(</op:operator><name>output_type</name> <op:operator>==</op:operator> <name>COMMON</name> <op:operator>||</op:operator> <name>output_type</name> <op:operator>==</op:operator> <name>INSERT</name><op:operator>)</op:operator> <op:operator>&amp;&amp;</op:operator> <name>output_end</name> <op:operator>==</op:operator> <op:operator>(</op:operator><name>signed</name><op:operator>)</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>
        <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr>;</expr_stmt>

        <if>if<condition>(<expr><name>output_type</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else <if>if<condition>(<expr><name>output_type</name> <op:operator>==</op:operator> <name>INSERT</name></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if></else></if>

        <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

      }</block></then></if>

      <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)getRealCurrentNode(reader_old)-&gt;name</argument>, <argument>"unit"</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <return>return;</return></then></if>

      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <if>if<condition>(<expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>output_type</name> <op:operator>=</op:operator> <name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

        <if>if<condition>(<expr><name>output_type</name> <op:operator>==</op:operator> <name>COMMON</name></expr>)</condition><then> <block>{

          <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
          <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">2</lit:literal></expr>;</expr_stmt>
        }</block></then>
        <else>else <block>{

          <comment type="line">// first two lines were not here not sure why not</comment>
          <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>
          <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr>;</expr_stmt>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
          <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
        }</block></else></if>
      }</block></then></if>

      <if>if<condition>(<expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>output_type</name> <op:operator>=</op:operator> <name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

        <if>if<condition>(<expr><name>output_type</name> <op:operator>==</op:operator> <name>COMMON</name></expr>)</condition><then> <block>{

          <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
          <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">2</lit:literal></expr>;</expr_stmt>
        }</block></then>
        <else>else <block>{

          <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>
          <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr>;</expr_stmt>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
          <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
        }</block></else></if>

      }</block></then></if>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <if>if<condition>(<expr><name>mark_open</name> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>
        <expr_stmt><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>)</op:operator><index>[<expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr>]</index> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
        <expr_stmt><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>)</op:operator><index>[<expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr>]</index> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
        <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr>;</expr_stmt>

      }</block></then></if>

      <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    }</block></else></if>
  }</block></while>

  <expr_stmt><expr><op:operator>++</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>
  <expr_stmt><expr><op:operator>++</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

}</block></diff:new></function>

<diff:old><comment type="line">// collect the differnces</comment></diff:old><diff:new><function><type><name>int</name></type> <name>merge_same_line</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>, <param><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diff</name></decl></param>, <param><decl><type><name>bool</name> <type:modifier>*</type:modifier></type> <name>mark_open</name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>int</name></type> <name>not_done_old</name> =<init> <expr><lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

    <comment type="line">// Output old then node then common</comment>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>DELETE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><name>not_done_old</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <if>if<condition>(<expr><op:operator>(</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator>
       <op:operator>||</op:operator> <op:operator>(</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr>)</condition><then> <block>{

      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><op:operator>*</op:operator><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
    }</block></then></if>

  }</block></then></if>

  <decl_stmt><decl><type><name>int</name></type> <name>not_done_new</name> =<init> <expr><lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

    <comment type="line">// Output new then node then common</comment>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><name>not_done_new</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    
    <if>if<condition>(<expr><op:operator>(</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator>
       <op:operator>||</op:operator> <op:operator>(</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr>)</condition><then> <block>{
    
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><op:operator>*</op:operator><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
    }</block></then></if>

  }</block></then></if>

  <comment type="line">// check if that open corrected and they now match if do return</comment>
  <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)getRealCurrentNode(reader_old)-&gt;name</argument>, <argument>(const char *)getRealCurrentNode(reader_new)-&gt;name</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
    <return>return <expr><lit:literal type="number">0</lit:literal></expr>;</return></then></if>

  <if>if<condition>(<expr><op:operator>*</op:operator><name>mark_open</name> <op:operator>&amp;&amp;</op:operator> <name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <name>COMMON</name>
     <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf_old-&gt;context-&gt;back()-&gt;name</argument>, <argument>(const char *)rbuf_new-&gt;context-&gt;back()-&gt;name</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{
    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"HERE\n"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><op:operator>*</op:operator><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>

  }</block></then> <else>else <block>{

    <comment type="line">// chew up common and exit</comment>
    <decl_stmt><decl><type><name>int</name></type> <name>offset</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</decl_stmt>
    <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then>
      <while>while<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf_old-&gt;context-&gt;back()-&gt;name</argument>, <argument>(const char *)rbuf_new-&gt;context-&gt;back()-&gt;name</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition> <block>{

        <expr_stmt><expr><name>not_done_old</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></while></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then>

      <while>while<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf_new-&gt;context-&gt;back()-&gt;name</argument>, <argument>(const char *)rbuf_new-&gt;context-&gt;back()-&gt;name</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition> <block>{

        <expr_stmt><expr><name>not_done_new</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></while></then></if></else></if>

  }</block></else></if>

  <comment type="line">// check last open diff and use to decide which goes first</comment>
  <if>if<condition>(<expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <name>INSERT</name></expr>)</condition><then> <block>{

    <comment type="line">// while in close and closing old or new element continue</comment>
    <while>while<condition>(<expr><name>not_done_new</name> <op:operator>&amp;&amp;</op:operator> <op:operator>!</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>&amp;&amp;</op:operator> <name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <name>INSERT</name> 
          <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition> <block>{

      <comment type="line">// update and output stuff</comment>
      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done_new</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    }</block></while>

    <if>if<condition>(<expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call></expr>)</condition><then> <block>{

      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done_new</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

    }</block></then></if>

    <comment type="line">// while in close and closing old or new element continue</comment>
    <while>while<condition>(<expr><name>not_done_old</name> <op:operator>&amp;&amp;</op:operator> <op:operator>!</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>&amp;&amp;</op:operator> <name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <name>DELETE</name>
          <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition> <block>{


      <comment type="line">// update and output stuff</comment>
      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done_old</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    }</block></while>

    <if>if<condition>(<expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call></expr>)</condition><then> <block>{

      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done_old</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
    }</block></then></if>

  }</block></then> <else>else <block>{

    <comment type="line">// while in close and closing old or new element continue</comment>
    <while>while<condition>(<expr><name>not_done_old</name> <op:operator>&amp;&amp;</op:operator> <op:operator>!</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>&amp;&amp;</op:operator> <name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <name>DELETE</name>
                    <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition> <block>{


      <comment type="line">// update and output stuff</comment>
      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done_old</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    }</block></while>

    <if>if<condition>(<expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call></expr>)</condition><then> <block>{

      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done_old</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>


      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

    }</block></then></if>

      

    <comment type="line">// while in close and closing old or new element continue</comment>
    <while>while<condition>(<expr><name>not_done_new</name> <op:operator>&amp;&amp;</op:operator> <op:operator>!</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>&amp;&amp;</op:operator> <name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <name>INSERT</name>
                    <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition> <block>{

      <comment type="line">// update and output stuff</comment>
      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done_new</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    }</block></while>

    <if>if<condition>(<expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call></expr>)</condition><then> <block>{

      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done_new</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader_new</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

    }</block></then></if>

  }</block></else></if>

  <if>if<condition>(<expr><op:operator>(</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator>
     <op:operator>||</op:operator> <op:operator>(</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator>  <op:operator>&amp;&amp;</op:operator> <name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <name>COMMON</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><op:operator>*</op:operator><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
    }</block></then></if>


  <return>return <expr><lit:literal type="number">1</lit:literal></expr>;</return>

}</block></function></diff:new>
<diff:old><function><type><name>void</name></type> <name>collect_difference</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>int</name></type> <name>end_line</name></decl></param>)</parameter_list> <block>{

  <comment type="line">// save beginning of characters</comment>
  <decl_stmt><decl><type><name>unsigned</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>characters_start</name> =<init> <expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr></init></decl>;</decl_stmt>

  <comment type="line">// allocate new buffer</comment>
  <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>

  <decl_stmt><decl><type><name>int</name></type> <name>not_done</name> =<init> <expr><lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
  <while>while<condition>(<expr><name>not_done</name></expr>)</condition>

    <comment type="line">// look if in text node</comment>
    <if>if<condition>(<expr><call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_SIGNIFICANT_WHITESPACE</name> <op:operator>||</op:operator> <call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name></expr>)</condition><then> <block>{

      <comment type="line">// allocate character buffer if empty and set start of characters</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>characters_start</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>unsigned</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><call><name>xmlTextReaderConstValue</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>characters_start</name></expr>;</expr_stmt>
      }</block></then></if>

      <comment type="line">// cycle through characters</comment>
      <for>for (<init>;</init> <condition><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr></incr><op:operator>)</op:operator> <block>{

        <comment type="line">// separte non whitespace</comment>
        <if>if<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">' '</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\t'</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\r'</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition><then> <block>{

          <comment type="line">// output previous whitespace</comment>
          <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>!=</op:operator> <name>characters_start</name></expr>)</condition><then> <block>{
            <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>text</name> =<init> <expr><op:operator>new</op:operator> <name>xmlNode</name></expr></init></decl>;</decl_stmt>
            <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_TEXT</name></expr>;</expr_stmt>
            <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>name</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"text"</lit:literal></expr>;</expr_stmt>

            <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>content</name> =<init> <macro><name>strndup</name><argument_list>(<argument>(const char *)characters_start</argument>, <argument>rbuf-&gt;characters  - characters_start</argument>)</argument_list></macro></init></decl>;</decl_stmt>
            <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>content</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>content</name></expr>;</expr_stmt>
            <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>text</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <expr_stmt><expr><name>characters_start</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

          }</block></then></if>

          <while>while<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">' '</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\t'</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\r'</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition>
            <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt></while>

          <comment type="line">// output other</comment>
          <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>text</name> =<init> <expr><op:operator>new</op:operator> <name>xmlNode</name></expr></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_TEXT</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>name</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"text"</lit:literal></expr>;</expr_stmt>

          <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>content</name> =<init> <macro><name>strndup</name><argument_list>(<argument>(const char *)characters_start</argument>, <argument>rbuf-&gt;characters  - characters_start</argument>)</argument_list></macro></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>content</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>content</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>text</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          <expr_stmt><expr><name>characters_start</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

          <if>if<condition>(<expr><op:operator>!*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>)</condition><then>
            <break>break;</break></then></if>

        }</block></then></if>

        <comment type="line">// increase new line count and check if end of diff</comment>
        <if>if<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>==</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition><then> <block>{

          <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

          <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>text</name> =<init> <expr><op:operator>new</op:operator> <name>xmlNode</name></expr></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_TEXT</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>name</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"text"</lit:literal></expr>;</expr_stmt>

          <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>content</name> =<init> <macro><name>strndup</name><argument_list>(<argument>(const char *)characters_start</argument>, <argument>(rbuf-&gt;characters + 1) - characters_start</argument>)</argument_list></macro></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>content</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>content</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>text</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          <expr_stmt><expr><name>characters_start</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>

          <comment type="line">// check if end of diff and create text node for text fragment</comment>
          <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name> <op:operator>==</op:operator> <name>end_line</name></expr>)</condition><then> <block>{

            <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

            <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

              <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
              <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
            }</block></then></if>

            <return>return;</return>
          }</block></then></if>

        }</block></then></if>
      }</block></for>

      <comment type="line">// end and save text node if finished and get next node</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

        <comment type="line">// create new node and buffer it</comment>
        <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>!=</op:operator> <name>characters_start</name></expr>)</condition><then> <block>{

          <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>text</name> =<init> <expr><op:operator>new</op:operator> <name>xmlNode</name></expr></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_TEXT</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>name</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"text"</lit:literal></expr>;</expr_stmt>

          <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>content</name> =<init> <macro><name>strdup</name><argument_list>(<argument>(const char *)characters_start</argument>)</argument_list></macro></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>content</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>content</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>text</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        }</block></then></if>

        <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>

        <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></then></if>
    }</block></then>
    <else>else <block>{

      <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>node</name> =<init> <expr><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

      <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)node-&gt;name</argument>, <argument>"unit"</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <return>return;</return></then></if>

      <comment type="line">// save non-text node and get next node</comment>
      <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>node</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    }</block></else></if></while>

  <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

}</block></function></diff:old>

<diff:old><comment type="line">// output a single difference DELETE or INSERT</comment></diff:old><diff:new><function><type><name>void</name></type> <name>output_single</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_other</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>, <param><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>int</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diff</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>int</name></type> <name>end_line</name></decl></param>)</parameter_list> <block>{

  <if>if<condition>(<expr><name>end_line</name> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
    <return>return;</return></then></if>

  <decl_stmt><decl><type><name>unsigned</name> <name>int</name></type> <name>last_open</name></decl>;</decl_stmt>
  <for>for(<init><expr><name>last_open</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr>;</init> <condition><expr><name>last_open</name> <op:operator>&gt;</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>)</op:operator><index>[<expr><name>last_open</name></expr>]</index> <op:operator>==</op:operator> <name>operation</name></expr>;</condition> <incr><expr><op:operator>--</op:operator><name>last_open</name></expr></incr><op:operator>)</op:operator><empty_stmt>;</empty_stmt></for>

  <expr_stmt><expr><op:operator>++</op:operator><name>last_open</name></expr>;</expr_stmt>

  <decl_stmt><decl><type><name>bool</name></type> <name>mark_open</name> =<init> <expr><lit:literal type="boolean">false</lit:literal></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><name>last_open</name> <op:operator>==</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>

    <if>if<condition>(<expr><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
    <else>else
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if>

    <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>

  <decl_stmt><decl><type><name>unsigned</name> <name>int</name></type> <name>last_open_other</name></decl>;</decl_stmt>
  <for>for(<init><expr><name>last_open_other</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr>;</init> <condition><expr><name>last_open_other</name> <op:operator>&gt;</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>)</op:operator><index>[<expr><name>last_open_other</name></expr>]</index> <op:operator>==</op:operator> <name>COMMON</name></expr>;</condition> <incr><expr><op:operator>--</op:operator><name>last_open_other</name></expr></incr><op:operator>)</op:operator><empty_stmt>;</empty_stmt></for>

  <expr_stmt><expr><op:operator>++</op:operator><name>last_open_other</name></expr>;</expr_stmt>

  <decl_stmt><decl><type><name>int</name></type> <name>not_done</name> =<init> <expr><lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>output_type</name> =<init> <expr><name>operation</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>output_end</name> =<init> <expr><op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr></init></decl>;</decl_stmt>
  <while>while<condition>(<expr><name>not_done</name></expr>)</condition> <block>{

    <comment type="line">// look if in text node</comment>
    <if>if<condition>(<expr><call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_SIGNIFICANT_WHITESPACE</name> <op:operator>||</op:operator> <call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name></expr>)</condition><then> <block>{

      <comment type="line">// allocate character buffer if empty</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>)</condition><then> <block>{
        <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>unsigned</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><call><name>xmlTextReaderConstValue</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></then></if>

      <comment type="line">// cycle through characters</comment>
      <for>for (<init>;</init> <condition><expr><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr></incr><op:operator>)</op:operator> <block>{

        <comment type="line">// escape characters or print out character</comment>
        <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&amp;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;amp;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&lt;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;lt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else <if>if <condition>(<expr><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>==</op:operator> <lit:literal type="char">'&gt;'</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&amp;gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr></argument>, <argument><expr><lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if></else></if></else></if>

        <comment type="line">// increase new line count and check if done</comment>
        <if>if<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>==</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition><then> <block>{

          <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

          <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name> <op:operator>==</op:operator> <name>end_line</name></expr>)</condition><then> <block>{

            <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

            <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

              <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
              <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            }</block></then></if>

            <if>if<condition>(<expr><name>mark_open</name></expr>)</condition><then>
              <if>if<condition>(<expr><name>output_type</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>
                <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
              <else>else
                <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if></then></if>

            <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
    
            <return>return;</return>
          }</block></then></if>

        }</block></then></if>

      }</block></for>

      <comment type="line">// end text node if finished and get next node</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>

        <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></then></if>
    }</block></then>
    <else>else <block>{

      <if>if<condition>(<expr><name>output_end</name> <op:operator>==</op:operator> <op:operator>(</op:operator><name>signed</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>
        <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr>;</expr_stmt>

        <if>if<condition>(<expr><name>output_type</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else <if>if<condition>(<expr><name>output_type</name> <op:operator>==</op:operator> <name>INSERT</name></expr>)</condition><then>
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
        <else>else
          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if></else></if>

            <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

      }</block></then></if>

      <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)getRealCurrentNode(reader)-&gt;name</argument>, <argument>"unit"</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <return>return;</return></then></if>

      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>output_type</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

        <if>if<condition>(<expr><name>output_type</name> <op:operator>!=</op:operator> <name>COMMON</name></expr>)</condition><then> <block>{

          <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
          <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">2</lit:literal></expr>;</expr_stmt>
        }</block></then>
        <else>else <block>{

          <comment type="line">// must end both streams if common</comment>
          <if>if<condition>(<expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call>
              <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;context-&gt;back()-&gt;name</argument>, <argument>(const char *)rbuf_other-&gt;context-&gt;back()-&gt;name</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

            <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>
            <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr>;</expr_stmt>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:common&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <expr_stmt><expr><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

            <if>if<condition>(<expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

              <expr_stmt><expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
              <expr_stmt><expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
              <expr_stmt><expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
            }</block></then></if>
          }</block></then></if>
        }</block></else></if>

        }</block></then><else>else <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <name>COMMON</name> 
                 <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name>
                 <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;context-&gt;back()-&gt;name</argument>, <argument>(const char *)rbuf_other-&gt;context-&gt;back()-&gt;name</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

          <if>if<condition>(<expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>!=</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

            <expr_stmt><expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
            <expr_stmt><expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
            <expr_stmt><expr><name>rbuf_other</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
          }</block></then></if>
        }</block></then></if></else></if>

      <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf</name></expr></argument>, <argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf</name></expr></argument>, <argument><expr><name>reader</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf</name></expr></argument>, <argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <if>if<condition>(<expr><name>mark_open</name> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>mark_open</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>
        <expr_stmt><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>)</op:operator><index>[<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr>]</index> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
        <expr_stmt><expr><name>output_end</name> <op:operator>=</op:operator> <op:operator>-</op:operator><lit:literal type="number">2</lit:literal></expr>;</expr_stmt>

      }</block></then></if>

      <comment type="line">// output non-text node and get next node</comment>
      <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    }</block></else></if>
  }</block></while>

  <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

}</block></function></diff:new>
<diff:old><function><type><name>void</name></type> <name>output_single</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list> <block>{

  <comment type="line">//fprintf(stderr, "HERE_SINGLE\n");</comment>

  <decl_stmt><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name> =<init> <expr><name>edit</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name> <op:operator>?</op:operator> <name>rbuf_old</name> : <name>rbuf_new</name></expr></init></decl>;</decl_stmt>

  <comment type="line">// output starting diff tag</comment>
  <if>if<condition>(<expr><name>edit</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then> <block>{

    <comment type="line">// output diff tag start</comment>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <comment type="block">/*
      if(rbuf-&gt;open_diff-&gt;back()-&gt;operation != DELETE)
      output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

      rbuf-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
    */</comment>

  }</block></then><else>else <block>{

    <comment type="line">// output diff tag start</comment>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <comment type="block">/*
      if(rbuf-&gt;open_diff-&gt;back()-&gt;operation != INSERT)
      output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

      rbuf-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
    */</comment>
  }</block></else></if>

  <comment type="block">/*  xmlNodePtr node;
      if(rbuf-&gt;open_diff-&gt;size() &gt; 1 &amp;&amp; rbuf-&gt;open_diff-&gt;back()-&gt;operation == edit-&gt;operation)
      node = (*rbuf-&gt;open_diff)[rbuf-&gt;open_diff-&gt;size() - 2]-&gt;open_tags-&gt;back();
      else
      node = rbuf-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;back();

      // output diff outputting until identified open tag
      */</comment>

  <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>bnode</name> =<init> <expr><name>NULL</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>unsigned</name> <name>int</name></type> <name>i</name></decl>;</decl_stmt>
  <for>for(<init><expr><name>i</name> <op:operator>=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator> <block>{


    <expr_stmt><expr><name>bnode</name> <op:operator>=</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>)</op:operator><index>[<expr><name>i</name></expr>]</index></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>bnode</name></expr></argument>, <argument><expr><name>edit</name><op:operator>-&gt;</op:operator><name>operation</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  }</block></for>

  <comment type="line">// output ending diff tags</comment>
  <comment type="line">// output starting diff tag</comment>
  <if>if<condition>(<expr><name>edit</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then> <block>{

    <comment type="line">// output diff tag start</comment>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <comment type="block">/*
      if(rbuf-&gt;open_diff-&gt;back()-&gt;operation == DELETE)
      rbuf-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;

      output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);
    */</comment>

  }</block></then><else>else <block>{

    <comment type="line">// output diff tag start</comment>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <comment type="block">/*
      if(rbuf-&gt;open_diff-&gt;back()-&gt;operation == INSERT)
      rbuf-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;

      output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
    */</comment>

  }</block></else></if>

  <comment type="block">/*
  // output remaining nodes on line
  for(; i &lt; rbuf-&gt;buffer-&gt;size(); ++i)
  output_handler(rbuf_old, rbuf_new, (*rbuf-&gt;buffer)[i], edit-&gt;operation, writer);
  */</comment>

  <comment type="line">//fprintf(stderr, "HERE\n");</comment>
}</block></function></diff:old>

<diff:old><function><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNodePtr</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>create_node_set</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNodePtr</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>node_sets</name> =<init> <expr><op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNodePtr</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr></init></decl>;</decl_stmt>

  <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator> <block>{

    <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name> <argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>node_set</name> =<init> <expr><op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name> <argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr></init></decl>;</decl_stmt>

      <if>if<condition>(<expr><op:operator>(</op:operator><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal><op:operator>)</op:operator> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name>
         <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;name</argument>, <argument>"name"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal>
         <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i + 2)-&gt;name</argument>, <argument>"name"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <comment type="line">//fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)rbuf-&gt;diff_nodes-&gt;at(i));</comment>
        <comment type="line">//fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)rbuf-&gt;diff_nodes-&gt;at(i + 1));</comment>
        <comment type="line">//fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)rbuf-&gt;diff_nodes-&gt;at(i + 2));</comment>

        <expr_stmt><expr><name>i</name> <op:operator>+=</op:operator> <lit:literal type="number">2</lit:literal></expr>;</expr_stmt>

      }</block></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal><op:operator>)</op:operator> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name>
         <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;name</argument>, <argument>"operator"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal>
         <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i + 2)-&gt;name</argument>, <argument>"operator"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><name>i</name> <op:operator>+=</op:operator> <lit:literal type="number">2</lit:literal></expr>;</expr_stmt>

      }</block></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal><op:operator>)</op:operator> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name>
         <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;name</argument>, <argument>"modifier"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal>
         <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i + 2)-&gt;name</argument>, <argument>"modifier"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><name>i</name> <op:operator>+=</op:operator> <lit:literal type="number">2</lit:literal></expr>;</expr_stmt>

      }</block></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal><op:operator>)</op:operator> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name>
         <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;name</argument>, <argument>"literal"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal>
         <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i + 2)-&gt;name</argument>, <argument>"literal"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name> <op:operator>+</op:operator> <lit:literal type="number">2</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><name>i</name> <op:operator>+=</op:operator> <lit:literal type="number">2</lit:literal></expr>;</expr_stmt>

      }</block></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name>
                <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;name</argument>, <argument>"param"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

        <for>for(<init>;</init><condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> 
              <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>!=</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name>
              <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;name</argument>, <argument>"param"</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>;</condition>
            <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
          <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></for>

      }</block></then> <else>else
        <comment type="block">/*
        if((xmlReaderTypes)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;type == XML_READER_TYPE_ELEMENT
                &amp;&amp; strcmp((const char *)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;name, "expr") == 0) {

        for(;i &lt; rbuf-&gt;diff_nodes-&gt;size() 
              &amp;&amp; (xmlReaderTypes)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;type != XML_READER_TYPE_END_ELEMENT
              &amp;&amp; strcmp((const char *)rbuf-&gt;diff_nodes-&gt;at(i)-&gt;name, "expr") == 0;
            ++i)
          node_set-&gt;push_back(rbuf-&gt;diff_nodes-&gt;at(i));

          } else*/</comment> <block>{
        
        <expr_stmt><expr><name>node_set</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>diff_nodes</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></else></if></else></if></else></if></else></if></else></if>

      <expr_stmt><expr><name>node_sets</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>node_set</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  }</block></for>

  <return>return <expr><name>node_sets</name></expr>;</return>

}</block></function></diff:old><diff:new><comment type="line">// collect the differnces</comment></diff:new>
<diff:new><function><type><name>void</name></type> <name>collect_difference</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>int</name></type> <name>end_line</name></decl></param>)</parameter_list> <block>{

  <comment type="line">// save beginning of characters</comment>
  <decl_stmt><decl><type><name>unsigned</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>characters_start</name> =<init> <expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr></init></decl>;</decl_stmt>

  <comment type="line">// allocate new buffer</comment>
  <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNode</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>not_done</name> =<init> <expr><lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
  <while>while<condition>(<expr><name>not_done</name></expr>)</condition>

    <comment type="line">// look if in text node</comment>
    <if>if<condition>(<expr><call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_SIGNIFICANT_WHITESPACE</name> <op:operator>||</op:operator> <call><name>xmlTextReaderNodeType</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name></expr>)</condition><then> <block>{

      <comment type="line">// allocate character buffer if empty and set start of characters</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><name>characters_start</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>unsigned</name> <name>char</name> <op:operator>*</op:operator><op:operator>)</op:operator><call><name>xmlTextReaderConstValue</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>characters_start</name></expr>;</expr_stmt>
      }</block></then></if>

      <comment type="line">// cycle through characters</comment>
      <for>for (<init>;</init> <condition><expr><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr></incr><op:operator>)</op:operator> <block>{

        <comment type="line">// separte non whitespace</comment>
        <if>if<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">' '</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\t'</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\r'</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition><then> <block>{

          <comment type="line">// output previous whitespace</comment>
          <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>!=</op:operator> <name>characters_start</name></expr>)</condition><then> <block>{
            <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>text</name> =<init> <expr><op:operator>new</op:operator> <name>xmlNode</name></expr></init></decl>;</decl_stmt>
            <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_TEXT</name></expr>;</expr_stmt>
            <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>name</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"text"</lit:literal></expr>;</expr_stmt>

            <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>content</name> =<init> <macro><name>strndup</name><argument_list>(<argument>(const char *)characters_start</argument>, <argument>rbuf-&gt;characters  - characters_start</argument>)</argument_list></macro></init></decl>;</decl_stmt>
            <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>content</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>content</name></expr>;</expr_stmt>
            <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>text</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <expr_stmt><expr><name>characters_start</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

          }</block></then></if>

          <while>while<condition>(<expr><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>!=</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">' '</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\t'</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\r'</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>!=</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition>
            <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt></while>

          <comment type="line">// output other</comment>
          <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>text</name> =<init> <expr><op:operator>new</op:operator> <name>xmlNode</name></expr></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_TEXT</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>name</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"text"</lit:literal></expr>;</expr_stmt>

          <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>content</name> =<init> <macro><name>strndup</name><argument_list>(<argument>(const char *)characters_start</argument>, <argument>rbuf-&gt;characters  - characters_start</argument>)</argument_list></macro></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>content</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>content</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>text</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          <expr_stmt><expr><name>characters_start</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

          <if>if<condition>(<expr><op:operator>!*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>)</condition><then>
            <break>break;</break></then></if>

        }</block></then></if>

        <comment type="line">// increase new line count and check if end of diff</comment>
        <if>if<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator> <op:operator>==</op:operator> <lit:literal type="char">'\n'</lit:literal></expr>)</condition><then> <block>{
          <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

          <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>text</name> =<init> <expr><op:operator>new</op:operator> <name>xmlNode</name></expr></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_TEXT</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>name</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"text"</lit:literal></expr>;</expr_stmt>

          <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>content</name> =<init> <macro><name>strndup</name><argument_list>(<argument>(const char *)characters_start</argument>, <argument>(rbuf-&gt;characters + 1) - characters_start</argument>)</argument_list></macro></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>content</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>content</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>text</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          <expr_stmt><expr><name>characters_start</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>

          <comment type="line">// check if end of diff and create text node for text fragment</comment>
          <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name> <op:operator>==</op:operator> <name>end_line</name></expr>)</condition><then> <block>{

            <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name></expr>;</expr_stmt>

            <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

              <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>
              <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
            }</block></then></if>

            <return>return;</return>
          }</block></then></if>

        }</block></then></if>
      }</block></for>

      <comment type="line">// end and save text node if finished and get next node</comment>
      <if>if<condition>(<expr><op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name><op:operator>)</op:operator></expr>)</condition><then> <block>{

        <comment type="line">// create new node and buffer it</comment>
        <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>!=</op:operator> <name>characters_start</name></expr>)</condition><then> <block>{

          <decl_stmt><decl><type><name>xmlNode</name> <type:modifier>*</type:modifier></type> <name>text</name> =<init> <expr><op:operator>new</op:operator> <name>xmlNode</name></expr></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_TEXT</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>name</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>const</name> <name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><lit:literal type="string">"text"</lit:literal></expr>;</expr_stmt>

          <decl_stmt><decl><type><name>const</name> <name>char</name> <type:modifier>*</type:modifier></type> <name>content</name> =<init> <macro><name>strdup</name><argument_list>(<argument>(const char *)characters_start</argument>)</argument_list></macro></init></decl>;</decl_stmt>
          <expr_stmt><expr><name>text</name><op:operator>-&gt;</op:operator><name>content</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlChar</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>content</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>text</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        }</block></then></if>

        <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>characters</name> <op:operator>=</op:operator> <name>NULL</name></expr>;</expr_stmt>

        <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></then></if>
    }</block></then>
    <else>else <block>{

      <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)getRealCurrentNode(reader)-&gt;name</argument>, <argument>"unit"</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <break>break;</break></then></if>

      <comment type="line">// do not update closes.</comment>
      <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

        <expr_stmt><expr><call><name>update_context</name><argument_list>(<argument><expr><name>rbuf</name></expr></argument>, <argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><call><name>update_in_diff</name><argument_list>(<argument><expr><name>rbuf</name></expr></argument>, <argument><expr><name>reader</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><call><name>update_issued_diff</name><argument_list>(<argument><expr><name>rbuf</name></expr></argument>, <argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      }</block></then></if>

      <comment type="line">// save non-text node and get next node</comment>
      <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>not_done</name> <op:operator>=</op:operator> <call><name>xmlTextReaderRead</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    }</block></else></if></while>

  <expr_stmt><expr><op:operator>++</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>line_number</name></expr>;</expr_stmt>

}</block></function></diff:new>
<diff:new>
<comment type="line">// output a single difference DELETE or INSERT</comment>
<function><type><name>void</name></type> <name>output_single</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list> <block>{

  <comment type="line">// may be complete whitespace need to check for that</comment>
  <comment type="line">// find if in same diff type</comment>
  <comment type="line">// find the last open tag that needs to be closed</comment>
  <decl_stmt><decl><type><name>unsigned</name> <name>int</name></type> <name>last_open</name></decl>;</decl_stmt>
  <for>for(<init><expr><name>last_open</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr>;</init> <condition><expr><name>last_open</name> <op:operator>&gt;</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>)</op:operator><index>[<expr><name>last_open</name></expr>]</index> <op:operator>==</op:operator> <name>edit</name><op:operator>-&gt;</op:operator><name>operation</name></expr>;</condition> <incr><expr><op:operator>--</op:operator><name>last_open</name></expr></incr><op:operator>)</op:operator><empty_stmt>;</empty_stmt></for>

  <decl_stmt><decl><type><name>bool</name></type> <name>issued_diff</name> =<init> <expr><op:operator>(</op:operator><name>last_open</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator> <op:operator>!=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>&amp;&amp;</op:operator> <op:operator>!</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>)</op:operator><index>[<expr><name>last_open</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>]</index></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><op:operator>(</op:operator><name>last_open</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator> <op:operator>!=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>)</condition><then>
    <expr_stmt><expr><op:operator>++</op:operator><name>last_open</name></expr>;</expr_stmt></then></if>

  <if>if<condition>(<expr><name>issued_diff</name></expr>)</condition><then>
    <expr_stmt><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>)</op:operator><index>[<expr><name>last_open</name></expr>]</index> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt></then></if>

  <comment type="line">// output starting diff tag</comment>
  <if>if<condition>(<expr><name>issued_diff</name> <op:operator>&amp;&amp;</op:operator> <name>edit</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>

    <comment type="line">// output diff tag start</comment>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
  <else>else <if>if<condition>(<expr><name>issued_diff</name></expr>)</condition><then>

    <comment type="line">// output diff tag start</comment>
    <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then></if></else></if>

  <comment type="line">//last_open;</comment>
  <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>node</name> =<init> <expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>)</op:operator><index>[<expr><name>last_open</name></expr>]</index></expr></init></decl>;</decl_stmt>

  <comment type="line">// count number same open</comment>
  <decl_stmt><decl><type><name>int</name></type> <name>count</name> =<init> <expr><lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
  <for>for(<init><decl><type><name>unsigned</name> <name>int</name></type> <name>i</name> =<init> <expr><name>last_open</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator> <block>{

    <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>bnode</name> =<init> <expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>)</op:operator><index>[<expr><name>i</name></expr>]</index></expr></init></decl>;</decl_stmt>

    <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>bnode</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name> <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)node-&gt;name</argument>, <argument>(const char *)bnode-&gt;name</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
      <expr_stmt><expr><op:operator>++</op:operator><name>count</name></expr>;</expr_stmt></then></if>

  }</block></for>

  <comment type="line">// output diff outputting until identified open tag</comment>
  <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>bnode</name> =<init> <expr><name>NULL</name></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>unsigned</name> <name>int</name></type> <name>i</name></decl>;</decl_stmt>

  <decl_stmt><decl><type><name>bool</name></type> <name>output_diff</name> =<init> <expr><lit:literal type="boolean">false</lit:literal></expr></init></decl>;</decl_stmt>

  <for>for(<init><expr><name>i</name> <op:operator>=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator> <block>{

    <expr_stmt><expr><name>bnode</name> <op:operator>=</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>i</name></expr>]</index></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><name>bnode</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>bnode</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name> <op:operator>&amp;&amp;</op:operator> <macro><name>strcmp</name><argument_list>(<argument>(const char *)node-&gt;name</argument>, <argument>(const char *)bnode-&gt;name</argument>)</argument_list></macro> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

      <expr_stmt><expr><op:operator>--</op:operator><name>count</name></expr>;</expr_stmt>
      <if>if<condition>(<expr><name>count</name> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

        <expr_stmt><expr><op:operator>++</op:operator><name>i</name></expr>;</expr_stmt>
        <expr_stmt><expr><name>output_diff</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt>
        <break>break;</break>
      }</block></then></if>

    }</block></then></if>

  }</block></for>

  <comment type="line">// may need to be output until close</comment>
  <comment type="line">// check if last node is text node and output</comment>
  <if>if<condition>(<expr><name>i</name> <op:operator>==</op:operator> <op:operator>(</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr>)</condition><then> <block>{
    <expr_stmt><expr><name>bnode</name> <op:operator>=</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>i</name></expr>]</index></expr>;</expr_stmt>
    <if>if<condition>(<expr><name>bnode</name> <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>bnode</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name></expr>)</condition><then> <block>{

      <expr_stmt><expr><op:operator>++</op:operator><name>i</name></expr>;</expr_stmt>
      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><name>bnode</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    }</block></then></if>
  }</block></then></if>

  <comment type="line">// output ending diff tags</comment>
  <if>if<condition>(<expr><name>output_diff</name></expr>)</condition><then>
    <if>if<condition>(<expr><name>edit</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:old&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></then>
    <else>else

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;/diff:new&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt></else></if></then></if>

  <comment type="line">// check if in common may need to issue new common</comment>
  <comment type="line">// output remaining nodes on line</comment>
  <for>for(<init>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
    <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>i</name></expr>]</index></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for>


  <for>for(<init><expr><name>i</name> <op:operator>=</op:operator> <lit:literal type="number">0</lit:literal></expr>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
    <if>if<condition>(<expr><call>(<name>xmlReaderTypes</name>)<argument_list>(<argument><expr><op:operator>*</op:operator><name>rbuf</name><op:operator>-&gt;</op:operator><name>buffer</name></expr></argument>)</argument_list></call><index>[<expr><name>i</name></expr>]</index><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{

      <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
      <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

    }</block></then></if></for>
}</block></function>

</diff:new><comment type="line">// output a change</comment>
<function><type><name>void</name></type> <name>output_double</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list> <diff:old><block>{

  <comment type="line">//fprintf(stderr, "HERE_DOUBLE\n");</comment>

  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNodePtr</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>node_sets_old</name> =<init> <expr><call><name>create_node_set</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument><name>xmlNodePtr</name></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>node_sets_new</name> =<init> <expr><call><name>create_node_set</name><argument_list>(<argument><expr><name>rbuf_new</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

  <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit_script</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>distance</name> =<init> <expr><call><name>shortest_edit_script</name><argument_list>(<argument><expr><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>, <argument><expr><op:operator>(</op:operator><name>void</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>node_sets_old</name></expr></argument>, <argument><expr><name>node_sets_new</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>,
                                      <argument><expr><op:operator>(</op:operator><name>void</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>node_sets_new</name></expr></argument>, <argument><expr><name>node_set_compare</name></expr></argument>, <argument><expr><name>node_set_index</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>edit_script</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><name>distance</name> <op:operator>&lt;</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Error with shortest edit script"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>exit</name><argument_list>(<argument><expr><name>distance</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>

  <decl_stmt><decl><type><name>int</name></type> <name>last_diff</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edits</name> =<init> <expr><name>edit_script</name></expr></init></decl>;</decl_stmt>
  <for>for (<init>;</init> <condition><expr><name>edits</name></expr>;</condition> <incr><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr></incr><op:operator>)</op:operator> <block>{

    <comment type="block">/*
    // output diff tag start
    //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"start\"/&gt;"));
    if(rbuf_old-&gt;open_diff-&gt;back()-&gt;operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

    rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
    */</comment>
    <comment type="line">// add preceeding unchanged</comment>
    <if>if<condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>
      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
          <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></for></then>

    <else>else
      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
          <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></for></else></if>

    <comment type="block">/*
      if(rbuf_old-&gt;open_diff-&gt;back()-&gt;operation == COMMON &amp;&amp; rbuf_old-&gt;open_diff-&gt;size() &gt; 1)
      rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;

      output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);
    */</comment>

    <comment type="line">// detect and change</comment>
    <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit_next</name> =<init> <expr><name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr></init></decl>;</decl_stmt>
    <if>if<condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name> <op:operator>&amp;&amp;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name> <op:operator>!=</op:operator> <name>NULL</name> <op:operator>&amp;&amp;</op:operator> <name>edit_next</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>INSERT</name>
       <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator> <op:operator>==</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>)</condition><then> <block>{

      <comment type="line">//      fprintf(stderr, "HERE\n");</comment>

      <comment type="line">// look for pure whitespace change</comment>
      <if>if<condition>(<expr><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><lit:literal type="number">0</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>node_sets_new</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><lit:literal type="number">0</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><lit:literal type="number">0</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name>
         <op:operator>&amp;&amp;</op:operator> <name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal> <op:operator>&amp;&amp;</op:operator> <name>node_sets_new</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal>
         <op:operator>&amp;&amp;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal> <op:operator>&amp;&amp;</op:operator> <name>edit_next</name><op:operator>-&gt;</op:operator><name>length</name> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

        <decl_stmt><decl><type><name>int</name></type> <name>whitespace_length_old</name> =<init> <macro><name>strspn</name><argument_list>(<argument>(const char *)node_sets_old-&gt;at(edits-&gt;offset_sequence_one)-&gt;at(0)-&gt;content</argument>, <argument>" \t\r\n"</argument>)</argument_list></macro></init></decl>;</decl_stmt>
        <decl_stmt><decl><type><name>int</name></type> <name>whitespace_length_new</name> =<init> <macro><name>strspn</name><argument_list>(<argument>(const char *)node_sets_new-&gt;at(edit_next-&gt;offset_sequence_two)-&gt;at(0)-&gt;content</argument>, <argument>" \t\r\n"</argument>)</argument_list></macro></init></decl>;</decl_stmt>

        <decl_stmt><decl><type><name>xmlChar</name> <type:modifier>*</type:modifier></type> <name>content_old</name> =<init> <expr><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><lit:literal type="number">0</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>content</name></expr></init></decl>;</decl_stmt>
        <decl_stmt><decl><type><name>xmlChar</name> <type:modifier>*</type:modifier></type> <name>content_new</name> =<init> <expr><name>node_sets_new</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><lit:literal type="number">0</lit:literal></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><name>content</name></expr></init></decl>;</decl_stmt>

        <decl_stmt><decl><type><name>int</name></type> <name>size_old</name> =<init> <macro><name>strlen</name><argument_list>(<argument>(const char *)node_sets_old-&gt;at(edits-&gt;offset_sequence_one)-&gt;at(0)-&gt;content</argument>)</argument_list></macro></init></decl>;</decl_stmt>
        <decl_stmt><decl><type><name>int</name></type> <name>size_new</name> =<init> <macro><name>strlen</name><argument_list>(<argument>(const char *)node_sets_new-&gt;at(edit_next-&gt;offset_sequence_two)-&gt;at(0)-&gt;content</argument>)</argument_list></macro></init></decl>;</decl_stmt>

        <if>if<condition>(<expr><name>whitespace_length_old</name> <op:operator>==</op:operator> <name>size_old</name> <op:operator>&amp;&amp;</op:operator> <name>whitespace_length_new</name> <op:operator>==</op:operator> <name>size_new</name></expr>)</condition><then> <block>{

          <decl_stmt><decl><type><name>int</name></type> <name>end_old</name> =<init> <expr><name>size_old</name> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
          <decl_stmt><decl><type><name>int</name></type> <name>end_new</name> =<init> <expr><name>size_new</name> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>

          <while>while<condition>(<expr><name>end_old</name> <op:operator>&gt;=</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <name>end_new</name> <op:operator>&gt;=</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <name><name>content_old</name><index>[<expr><name>end_old</name></expr>]</index></name> <op:operator>==</op:operator> <name><name>content_new</name><index>[<expr><name>end_new</name></expr>]</index></name></expr>)</condition> <block>{

            <expr_stmt><expr><op:operator>--</op:operator><name>end_old</name></expr>;</expr_stmt>
            <expr_stmt><expr><op:operator>--</op:operator><name>end_new</name></expr>;</expr_stmt>
          }</block></while>

          <if>if<condition>(<expr><name>end_old</name> <op:operator>&gt;=</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

            <comment type="line">// output diff tag</comment>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
            <comment type="line">//xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old&gt;"));</comment>

            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>content_old</name></expr></argument>, <argument><expr><name>end_old</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <comment type="line">// output diff tag</comment>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
            <comment type="line">//xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;/diff:old&gt;"));</comment>

          }</block></then></if>

          <if>if<condition>(<expr><name>end_new</name> <op:operator>&gt;=</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

            <comment type="line">// output diff tag</comment>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
            <comment type="line">//xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:new&gt;"));</comment>

            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>content_new</name></expr></argument>, <argument><expr><name>end_new</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <comment type="line">// output diff tag</comment>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
            <comment type="line">//xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("/&lt;diff:new&gt;"));</comment>

          }</block></then></if>

          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>content_old</name> <op:operator>+</op:operator> <name>end_old</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>, <argument><expr><name>size_old</name> <op:operator>-</op:operator> <op:operator>(</op:operator><name>end_old</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr>;</expr_stmt>
          <continue>continue;</continue>
        }</block></then></if>

      }</block></then></if>

      <comment type="block">/*
        if(rbuf_old-&gt;output_diff-&gt;back()-&gt;operation == DELETE || rbuf_old-&gt;output_diff-&gt;back()-&gt;operation == COMMON) {
      */</comment>

      <comment type="line">// output diff tag start</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <comment type="block">/*
        if(rbuf_old-&gt;open_diff-&gt;back()-&gt;operation != DELETE)
        output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

        rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
      */</comment>

      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
          <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></for>

      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <comment type="block">/*
      // output diff tag start
      if(rbuf_old-&gt;open_diff-&gt;back()-&gt;operation == DELETE)
      rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;
      output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);
      */</comment>

      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      <comment type="block">/*
      // output diff tag
      if(rbuf_new-&gt;open_diff-&gt;back()-&gt;operation != INSERT)
      output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

      rbuf_new-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
      */</comment>

      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edit_next</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>node_sets_new</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
          <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>node_sets_new</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></for>

      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <comment type="block">/*
      // output diff tag start
      if(rbuf_new-&gt;open_diff-&gt;back()-&gt;operation == INSERT)
      rbuf_new-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;
      output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
      */</comment>
      <comment type="block">/*
        } else {

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:new status=\"start\"/&gt;"));

        for(int j = 0; j &lt; edit_next-&gt;length; ++j)
        output_handler(rbuf_old, rbuf_new, (*rbuf_new-&gt;buffer)[edit_next-&gt;offset_sequence_two + j], INSERT, writer);


        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:new status=\"end\"/&gt;"));

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"start\"/&gt;"));

        for(int j = 0; j &lt; edits-&gt;length; ++j)
        output_handler(rbuf_old, rbuf_new, (*rbuf_old-&gt;buffer)[edits-&gt;offset_sequence_one + j], DELETE, writer);

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"end\"/&gt;"));

        }
      */</comment>


      <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
      <expr_stmt><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr>;</expr_stmt>
      <continue>continue;</continue>
    }</block></then></if>

    <comment type="line">// handle pure delete or insert</comment>
    <switch>switch <condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name></expr>)</condition> <block>{

    <case>case <expr><name>INSERT</name></expr>:

      <block>{

        <comment type="line">//      fprintf(stderr, "HERE\n");</comment>
        <comment type="block">/*
          bool output_start = false;
          bool output_end = true;

          for(int j = 0; j &lt; edits-&gt;length; ++j) {

          if(!output_start &amp;&amp; output_peek(rbuf_old, rbuf_new, (*rbuf_new-&gt;buffer)[edits-&gt;offset_sequence_two + j], INSERT, writer)) {

          output_start = true;

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:new status=\"start\"/&gt;"));
          }

          if(output_start &amp;&amp; output_end &amp;&amp; rbuf_new-&gt;open_diff-&gt;back()-&gt;operation == COMMON
          &amp;&amp; (*rbuf_new-&gt;buffer)[edits-&gt;offset_sequence_two + j]-&gt;type == XML_READER_TYPE_END_ELEMENT) {

          output_end = false;

          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:new status=\"end\"/&gt;"));
          }

          output_handler(rbuf_old, rbuf_new, (*rbuf_new-&gt;buffer)[edits-&gt;offset_sequence_two + j], INSERT, writer);

          }

          if(output_start &amp;&amp; output_end)

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:new status=\"end\"/&gt;"));
        */</comment>

        <comment type="line">// output diff tag</comment>
        <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <comment type="block">/*
          if(rbuf_new-&gt;open_diff-&gt;back()-&gt;operation != INSERT)
          output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

          rbuf_new-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
        */</comment>

        <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
          <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>node_sets_new</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
            <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>node_sets_new</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></for>

        <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <comment type="block">/*
        // output diff tag
        if(rbuf_new-&gt;open_diff-&gt;back()-&gt;operation == INSERT)
        rbuf_new-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;
        output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
        */</comment>

        <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>

      }</block>

      <break>break;</break>

    </case><case>case <expr><name>DELETE</name></expr>:

      <block>{

        <comment type="line">//fprintf(stderr, "HERE\n");</comment>

        <comment type="block">/*
          bool output_start = false;
          bool output_end = true;

          for(int j = 0; j &lt; edits-&gt;length; ++j) {

          if(!output_start &amp;&amp; output_peek(rbuf_old, rbuf_new, (*rbuf_old-&gt;buffer)[edits-&gt;offset_sequence_one + j], DELETE, writer)) {

          output_start = true;

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"start\"/&gt;"));
          }

          if(output_start &amp;&amp; output_end &amp;&amp; rbuf_old-&gt;open_diff-&gt;back()-&gt;operation == COMMON
          &amp;&amp; (*rbuf_old-&gt;buffer)[edits-&gt;offset_sequence_one + j]-&gt;type == XML_READER_TYPE_END_ELEMENT) {

          output_end = false;
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"end\"/&gt;"));

          }

          output_handler(rbuf_old, rbuf_new, (*rbuf_old-&gt;buffer)[edits-&gt;offset_sequence_one + j], DELETE, writer);

          }

          if(output_start &amp;&amp; output_end)

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"end\"/&gt;"));
        */</comment>

        <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <comment type="block">/*
        // output diff tag start
        if(rbuf_old-&gt;open_diff-&gt;back()-&gt;operation != DELETE)
        output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

        rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
        */</comment>

        <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
          <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
            <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></for>

        <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <comment type="block">/*
        // output diff tag
        if(rbuf_old-&gt;open_diff-&gt;back()-&gt;operation == DELETE)
        rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;
        output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);
        */</comment>

        <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>

      }</block>

      <break>break;</break>
    </case>}</block></switch>

  }</block></for>

  <comment type="block">/*
  // output diff tag start
  //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;diff:old status=\"start\"/&gt;"));
  if(rbuf_old-&gt;open_diff-&gt;back()-&gt;operation != COMMON)
  output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = false;
  */</comment>


  <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
    <for>for(<init><decl><type><name>int</name></type> <name>i</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>i</name> <op:operator>&lt;</op:operator> <name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>i</name></expr></incr><op:operator>)</op:operator>
      <expr_stmt><expr><call><name>output_handler</name><argument_list>(<argument><expr><name>rbuf_old</name></expr></argument>, <argument><expr><name>rbuf_new</name></expr></argument>, <argument><expr><name>node_sets_old</name><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>j</name></expr></argument>)</argument_list></call><op:operator>-&gt;</op:operator><call><name>at</name><argument_list>(<argument><expr><name>i</name></expr></argument>)</argument_list></call></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></for>

  <comment type="block">/*
    if(rbuf_old-&gt;open_diff-&gt;back()-&gt;operation == COMMON &amp;&amp; rbuf_old-&gt;open_diff-&gt;size() &gt; 1)
    rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags-&gt;front()-&gt;marked = true;

    output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);
  */</comment>

  <expr_stmt><expr><call><name>free_shortest_edit_script</name><argument_list>(<argument><expr><name>edit_script</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>


}</block></diff:old><diff:new><block>{

  <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit_script</name></decl>;</decl_stmt>
  <decl_stmt><decl><type><name>int</name></type> <name>distance</name> =<init> <expr><call><name>shortest_edit_script</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>, <argument><expr><op:operator>(</op:operator><name>void</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name></expr></argument>, <argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr></argument>, <argument><expr><op:operator>(</op:operator><name>void</name> <op:operator>*</op:operator><op:operator>)</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>buffer</name></expr></argument>, <argument><expr><name>node_compare</name></expr></argument>, <argument><expr><name>node_index</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>edit_script</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><name>distance</name> <op:operator>&lt;</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

    <expr_stmt><expr><call><name>fprintf</name><argument_list>(<argument><expr><name>stderr</name></expr></argument>, <argument><expr><lit:literal type="string">"Error with shortest edit script"</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>exit</name><argument_list>(<argument><expr><name>distance</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>

  <decl_stmt><decl><type><name>int</name></type> <name>last_diff</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</decl_stmt>
  <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edits</name> =<init> <expr><name>edit_script</name></expr></init></decl>;</decl_stmt>
  <for>for (<init>;</init> <condition><expr><name>edits</name></expr>;</condition> <incr><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr></incr><op:operator>)</op:operator> <block>{

    <comment type="line">// add preceeding unchanged</comment>
    <if>if<condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then>
      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>j</name></expr>]</index></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></then>

    <else>else
      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>j</name></expr>]</index></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for></else></if>

    <comment type="line">// detect and change</comment>
    <decl_stmt><decl><type>struct <name>edit</name> <type:modifier>*</type:modifier></type> <name>edit_next</name> =<init> <expr><name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr></init></decl>;</decl_stmt>
    <if>if<condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name> <op:operator>&amp;&amp;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name> <op:operator>!=</op:operator> <name>NULL</name> <op:operator>&amp;&amp;</op:operator> <name>edit_next</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>INSERT</name>
       <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator> <op:operator>==</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>)</condition><then> <block>{

      <comment type="line">//      fprintf(stderr, "HERE\n");</comment>

      <comment type="line">// look for pure whitespace change</comment>
      <if>if<condition>(<expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>]</index><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name></expr>]</index><op:operator>-&gt;</op:operator><name>type</name>
         <op:operator>&amp;&amp;</op:operator> <call>(<name>xmlReaderTypes</name>)<argument_list>(<argument><expr><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name></expr></argument>)</argument_list></call><index>[<expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>]</index><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_TEXT</name>
         <op:operator>&amp;&amp;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal> <op:operator>&amp;&amp;</op:operator> <name>edit_next</name><op:operator>-&gt;</op:operator><name>length</name> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then> <block>{

        <decl_stmt><decl><type><name>int</name></type> <name>whitespace_length_old</name> =<init> <macro><name>strspn</name><argument_list>(<argument>(const char *)(*rbuf_old-&gt;buffer)[edits-&gt;offset_sequence_one]-&gt;content</argument>, <argument>" \t\r\n"</argument>)</argument_list></macro></init></decl>;</decl_stmt>
        <decl_stmt><decl><type><name>int</name></type> <name>whitespace_length_new</name> =<init> <macro><name>strspn</name><argument_list>(<argument>(const char *)(*rbuf_new-&gt;buffer)[edit_next-&gt;offset_sequence_two]-&gt;content</argument>, <argument>" \t\r\n"</argument>)</argument_list></macro></init></decl>;</decl_stmt>

        <decl_stmt><decl><type><name>xmlChar</name> <type:modifier>*</type:modifier></type> <name>content_old</name> =<init> <expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name></expr>]</index><op:operator>-&gt;</op:operator><name>content</name></expr></init></decl>;</decl_stmt>
        <decl_stmt><decl><type><name>xmlChar</name> <type:modifier>*</type:modifier></type> <name>content_new</name> =<init> <expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name></expr>]</index><op:operator>-&gt;</op:operator><name>content</name></expr></init></decl>;</decl_stmt>

        <decl_stmt><decl><type><name>int</name></type> <name>size_old</name> =<init> <macro><name>strlen</name><argument_list>(<argument>(const char *)(*rbuf_old-&gt;buffer)[edits-&gt;offset_sequence_one]-&gt;content</argument>)</argument_list></macro></init></decl>;</decl_stmt>
        <decl_stmt><decl><type><name>int</name></type> <name>size_new</name> =<init> <macro><name>strlen</name><argument_list>(<argument>(const char *)(*rbuf_new-&gt;buffer)[edit_next-&gt;offset_sequence_two]-&gt;content</argument>)</argument_list></macro></init></decl>;</decl_stmt>

        <if>if<condition>(<expr><name>whitespace_length_old</name> <op:operator>==</op:operator> <name>size_old</name> <op:operator>&amp;&amp;</op:operator> <name>whitespace_length_new</name> <op:operator>==</op:operator> <name>size_new</name></expr>)</condition><then> <block>{

          <decl_stmt><decl><type><name>int</name></type> <name>end_old</name> =<init> <expr><name>size_old</name> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>
          <decl_stmt><decl><type><name>int</name></type> <name>end_new</name> =<init> <expr><name>size_new</name> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr></init></decl>;</decl_stmt>

          <while>while<condition>(<expr><name>end_old</name> <op:operator>&gt;=</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <name>end_new</name> <op:operator>&gt;=</op:operator> <lit:literal type="number">0</lit:literal> <op:operator>&amp;&amp;</op:operator> <name><name>content_old</name><index>[<expr><name>end_old</name></expr>]</index></name> <op:operator>==</op:operator> <name><name>content_new</name><index>[<expr><name>end_new</name></expr>]</index></name></expr>)</condition> <block>{

            <expr_stmt><expr><op:operator>--</op:operator><name>end_old</name></expr>;</expr_stmt>
            <expr_stmt><expr><op:operator>--</op:operator><name>end_new</name></expr>;</expr_stmt>
          }</block></while>

          <if>if<condition>(<expr><name>end_old</name> <op:operator>&gt;=</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

            <comment type="line">// output diff tag</comment>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>content_old</name></expr></argument>, <argument><expr><name>end_old</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <comment type="line">// output diff tag</comment>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          }</block></then></if>

          <if>if<condition>(<expr><name>end_new</name> <op:operator>&gt;=</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{

            <comment type="line">// output diff tag</comment>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>content_new</name></expr></argument>, <argument><expr><name>end_new</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>

            <comment type="line">// output diff tag</comment>
            <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          }</block></then></if>

          <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><name>content_old</name> <op:operator>+</op:operator> <name>end_old</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr></argument>, <argument><expr><name>size_old</name> <op:operator>-</op:operator> <op:operator>(</op:operator><name>end_old</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal><op:operator>)</op:operator></expr></argument>)</argument_list></call></expr>;</expr_stmt>

          <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
          <expr_stmt><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr>;</expr_stmt>
          <continue>continue;</continue>
        }</block></then></if>
      }</block></then></if>

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>j</name></expr>]</index></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for>

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edit_next</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>edit_next</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>j</name></expr>]</index></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for>

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
      <expr_stmt><expr><name>edits</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>next</name></expr>;</expr_stmt>
      <continue>continue;</continue>
    }</block></then></if>

    <comment type="line">// handle pure delete or insert</comment>
    <switch>switch <condition>(<expr><name>edits</name><op:operator>-&gt;</op:operator><name>operation</name></expr>)</condition> <block>{

    <case>case <expr><name>INSERT</name></expr>:
      <comment type="line">//      fprintf(stderr, "HERE\n");</comment>
      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_two</name> <op:operator>+</op:operator> <name>j</name></expr>]</index></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for>

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:new status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <lit:literal type="number">1</lit:literal></expr>;</expr_stmt>
      <break>break;</break>
    </case><case>case <expr><name>DELETE</name></expr>:
      <comment type="line">//      fprintf(stderr, "HERE\n");</comment>

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"start\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <for>for(<init><decl><type><name>int</name></type> <name>j</name> =<init> <expr><lit:literal type="number">0</lit:literal></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
        <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>j</name></expr>]</index></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for>

      <comment type="line">// output diff tag</comment>
      <expr_stmt><expr><call><name>xmlTextWriterWriteRawLen</name><argument_list>(<argument><expr><name>writer</name></expr></argument>, <argument><expr><call><name>LITERALPLUSSIZE</name><argument_list>(<argument><expr><lit:literal type="string">"&lt;diff:old status=\"end\"/&gt;"</lit:literal></expr></argument>)</argument_list></call></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <expr_stmt><expr><name>last_diff</name> <op:operator>=</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>offset_sequence_one</name> <op:operator>+</op:operator> <name>edits</name><op:operator>-&gt;</op:operator><name>length</name></expr>;</expr_stmt>
      <break>break;</break>
    </case>}</block></switch>

  }</block></for>

  <for>for(<init><decl><type><name>unsigned</name> <name>int</name></type> <name>j</name> =<init> <expr><name>last_diff</name></expr></init></decl>;</init> <condition><expr><name>j</name> <op:operator>&lt;</op:operator> <name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call></expr>;</condition> <incr><expr><op:operator>++</op:operator><name>j</name></expr></incr><op:operator>)</op:operator>
    <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>buffer</name><op:operator>)</op:operator><index>[<expr><name>j</name></expr>]</index></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt></for>

  <expr_stmt><expr><call><name>free_shortest_edit_script</name><argument_list>(<argument><expr><name>edit_script</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

}</block></diff:new></function>

<function_decl><type><name>void</name></type> <name>addNamespace</name><parameter_list>(<param><decl><type><name>xmlNsPtr</name> <type:modifier>*</type:modifier></type> <name>nsDef</name></decl></param>, <param><decl><type><name>xmlNsPtr</name></type> <name>ns</name></decl></param>)</parameter_list>;</function_decl>

<comment type="line">// create srcdiff unit</comment>
<function><type><name>xmlNodePtr</name></type> <name>create_srcdiff_unit</name><parameter_list>(<param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_old</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader_new</name></decl></param>)</parameter_list> <block>{

  <comment type="line">// get units from source code</comment>
  <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>unit</name> =<init> <expr><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader_old</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <comment type="line">//xmlNodePtr unit_new = getRealCurrentNode(reader_new);</comment>

  <comment type="line">// add diff namespace</comment>
  <expr_stmt><expr><call><name>addNamespace</name><argument_list>(<argument><expr><op:operator>&amp;</op:operator><name>unit</name><op:operator>-&gt;</op:operator><name>nsDef</name></expr></argument>, <argument><expr><op:operator>&amp;</op:operator><name>diff</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <return>return <expr><name>unit</name></expr>;</return>
}</block></function>

<function><type><name>void</name></type> <name>addNamespace</name><parameter_list>(<param><decl><type><name>xmlNsPtr</name> <type:modifier>*</type:modifier></type> <name>nsDef</name></decl></param>, <param><decl><type><name>xmlNsPtr</name></type> <name>ns</name></decl></param>)</parameter_list> <block>{

  <decl_stmt><decl><type><name>xmlNsPtr</name></type> <name>namespaces</name> =<init> <expr><op:operator>*</op:operator><name>nsDef</name></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><name>namespaces</name></expr>)</condition><then> <block>{
    <for>for(<init>;</init> <condition><expr><name>namespaces</name><op:operator>-&gt;</op:operator><name>next</name></expr>;</condition> <incr><expr><name>namespaces</name> <op:operator>=</op:operator> <name>namespaces</name><op:operator>-&gt;</op:operator><name>next</name></expr></incr><op:operator>)</op:operator><empty_stmt>;</empty_stmt></for>

    <expr_stmt><expr><name>namespaces</name><op:operator>-&gt;</op:operator><name>next</name> <op:operator>=</op:operator> <name>ns</name></expr>;</expr_stmt>
  }</block></then>
  <else>else
    <expr_stmt><expr><op:operator>*</op:operator><name>nsDef</name> <op:operator>=</op:operator> <name>ns</name></expr>;</expr_stmt></else></if>

}</block></function>

<function><type><name>void</name></type> <diff:old><name>update_diff_stack</name><parameter_list>(<param><decl><type><name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>open_diff</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name> <type:modifier>*</type:modifier></type> <name>open_diffs</name></decl></param>, <param><decl><type><name>xmlNodePtr</name></type> <name>node</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>)</parameter_list></diff:old><diff:new><name>update_context</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>)</parameter_list></diff:new> <diff:old><block>{

  <if>if<condition>(<expr><name>node</name><op:operator>-&gt;</op:operator><name>extra</name> <op:operator>&amp;</op:operator> <lit:literal type="number">0x1</lit:literal></expr>)</condition><then>
    <return>return;</return></then></if>

  <if>if<condition>(<expr><name>open_diffs</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>!=</op:operator> <name>operation</name></expr>)</condition><then> <block>{

    <decl_stmt><decl><type>struct <name>open_diff</name> <type:modifier>*</type:modifier></type> <name>new_diff</name> =<init> <expr><op:operator>new</op:operator> struct <name>open_diff</name></expr></init></decl>;</decl_stmt>
    <expr_stmt><expr><name>new_diff</name><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>=</op:operator> <name>operation</name></expr>;</expr_stmt>
    <expr_stmt><expr><name>new_diff</name><op:operator>-&gt;</op:operator><name>open_tags</name> <op:operator>=</op:operator> <op:operator>new</op:operator> <name><name>std</name><op:operator>::</op:operator><name><name>vector</name><argument_list>&lt;<argument>struct <name>tag</name> <type:modifier>*</type:modifier></argument>&gt;</argument_list></name></name></expr>;</expr_stmt>

    <expr_stmt><expr><name>open_diffs</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>new_diff</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then></if>

  <comment type="line">//xmlNodePtr node = getRealCurrentNode(reader);</comment>
  <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

    <decl_stmt><decl><type>struct <name>tag</name> <type:modifier>*</type:modifier></type> <name>new_tag</name> =<init> <expr><op:operator>new</op:operator> struct <name>tag</name></expr></init></decl>;</decl_stmt>
    <expr_stmt><expr><name>new_tag</name><op:operator>-&gt;</op:operator><name>marked</name> <op:operator>=</op:operator> <lit:literal type="boolean">false</lit:literal></expr>;</expr_stmt>
    <expr_stmt><expr><name>new_tag</name><op:operator>-&gt;</op:operator><name>node</name> <op:operator>=</op:operator> <name>node</name></expr>;</expr_stmt>

    <expr_stmt><expr><name>open_diffs</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>new_tag</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{

    <if>if<condition>(<expr><name>open_diffs</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal> <op:operator>&amp;&amp;</op:operator> <name>open_diffs</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then>
      <return>return;</return></then></if>


    <expr_stmt><expr><name>open_diffs</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
  }</block></then></if></else></if>


  <comment type="line">//fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs-&gt;size());</comment>
  <comment type="line">//fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs-&gt;back()-&gt;open_tags-&gt;size());</comment>
  <if>if<condition>(<expr><name>open_diffs</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then> <block>{
    <expr_stmt><expr><name>open_diffs</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>

    <comment type="line">//fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs-&gt;size());</comment>
    <comment type="line">//fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs-&gt;back()-&gt;open_tags-&gt;size());</comment>
  }</block></then></if>
  <comment type="line">//fprintf(stderr, "HERE\n");</comment>

}</block></diff:old><diff:new><block>{

  <if>if<condition>(<expr><call><name>xmlTextReaderIsEmptyElement</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>)</condition><then>
    <return>return;</return></then></if>

  <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>node</name> =<init> <expr><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>node</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{

    <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then>
      <return>return;</return></then></if>

    <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>context</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
  }</block></then></if></else></if>
}</block></diff:new></function>

<function><type><name>void</name></type> <diff:old><name>output_handler</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlNodePtr</name></type> <name>node</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list></diff:old><diff:new><name>update_in_diff</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>, <param><decl><type><name>int</name></type> <name>indiff</name></decl></param>)</parameter_list></diff:new> <diff:old><block>{

  <comment type="block">/*
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, operation);
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf-&gt;output_diff-&gt;back()-&gt;operation);

    if(node-&gt;type == XML_READER_TYPE_TEXT)
    fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node-&gt;content);
    else
    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node-&gt;name);
  */</comment>

  <decl_stmt><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name> =<init> <expr><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name> <op:operator>?</op:operator> <name>rbuf_old</name> : <name>rbuf_new</name></expr></init></decl>;</decl_stmt>

  <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{
    <comment type="line">//    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node-&gt;name);</comment>

    <comment type="line">// heavily relaxed condition.  Tighten when fix diff to contain necessary elements</comment>
    <comment type="line">// mark nodes for output</comment>
    <decl_stmt><decl><type><name>int</name></type> <name>unmarked</name></decl>;</decl_stmt>
    <for>for(<init><expr><name>unmarked</name> <op:operator>=</op:operator> <name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr>;</init> <condition><expr><name>unmarked</name> <op:operator>&gt;</op:operator> <lit:literal type="number">0</lit:literal>
          <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>)</op:operator><index>[<expr><name>unmarked</name></expr>]</index><op:operator>-&gt;</op:operator><name>marked</name></expr>;</condition> <incr><expr><op:operator>--</op:operator><name>unmarked</name></expr></incr><op:operator>)</op:operator><empty_stmt>;</empty_stmt></for>

    <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>output_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>!=</op:operator> <name>COMMON</name></expr>)</condition><then> <block>{

      <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)(*rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags)[unmarked]-&gt;node-&gt;name</argument>, <argument>(const char *)node-&gt;name</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <expr_stmt><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>)</op:operator><index>[<expr><name>unmarked</name></expr>]</index><op:operator>-&gt;</op:operator><name>marked</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt></then></if>

    }</block></then> <else>else <block>{

      <if>if<condition>(<expr><name>operation</name> <op:operator>!=</op:operator> <name>INSERT</name></expr>)</condition><then>
        <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)(*rbuf_old-&gt;open_diff-&gt;back()-&gt;open_tags)[unmarked]-&gt;node-&gt;name</argument>, <argument>(const char *)node-&gt;name</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>)</op:operator><index>[<expr><name>unmarked</name></expr>]</index><op:operator>-&gt;</op:operator><name>marked</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt></then></if></then></if>

    }</block></else></if>

    <for>for(<init><expr><name>unmarked</name> <op:operator>=</op:operator> <name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>-</op:operator> <lit:literal type="number">1</lit:literal></expr>;</init> <condition><expr><name>unmarked</name> <op:operator>&gt;</op:operator> <lit:literal type="number">0</lit:literal>
          <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>)</op:operator><index>[<expr><name>unmarked</name></expr>]</index><op:operator>-&gt;</op:operator><name>marked</name></expr>;</condition> <incr><expr><op:operator>--</op:operator><name>unmarked</name></expr></incr><op:operator>)</op:operator><empty_stmt>;</empty_stmt></for>

    <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>output_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>!=</op:operator> <name>COMMON</name></expr>)</condition><then> <block>{

      <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)(*rbuf_new-&gt;open_diff-&gt;back()-&gt;open_tags)[unmarked]-&gt;node-&gt;name</argument>, <argument>(const char *)node-&gt;name</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <expr_stmt><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>)</op:operator><index>[<expr><name>unmarked</name></expr>]</index><op:operator>-&gt;</op:operator><name>marked</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt></then></if>

    }</block></then> <else>else <block>{

      <if>if<condition>(<expr><name>operation</name> <op:operator>!=</op:operator> <name>DELETE</name></expr>)</condition><then>
        <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)(*rbuf_new-&gt;open_diff-&gt;back()-&gt;open_tags)[unmarked]-&gt;node-&gt;name</argument>, <argument>(const char *)node-&gt;name</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
          <expr_stmt><expr><op:operator>(</op:operator><op:operator>*</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>)</op:operator><index>[<expr><name>unmarked</name></expr>]</index><op:operator>-&gt;</op:operator><name>marked</name> <op:operator>=</op:operator> <lit:literal type="boolean">true</lit:literal></expr>;</expr_stmt></then></if></then></if>

    }</block></else></if>

    <comment type="line">// output marked nodes</comment>
    <while>while<condition>(<expr><lit:literal type="number">1</lit:literal></expr>)</condition> <block>{

      <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>output_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>COMMON</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>COMMON</name>
             <op:operator>&amp;&amp;</op:operator> <name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>COMMON</name><op:operator>)</op:operator>
         <op:operator>&amp;&amp;</op:operator> <op:operator>(</op:operator><op:operator>!</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>marked</name>
             <op:operator>||</op:operator> <op:operator>!</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>marked</name><op:operator>)</op:operator></expr>)</condition><then>
        <return>return;</return></then></if>

      <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>output_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name>
         <op:operator>&amp;&amp;</op:operator> <name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>!</op:operator><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>marked</name></expr>)</condition><then>
        <return>return;</return></then></if>

      <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>output_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>INSERT</name>
         <op:operator>&amp;&amp;</op:operator> <name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>INSERT</name>
         <op:operator>&amp;&amp;</op:operator> <op:operator>!</op:operator><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>marked</name></expr>)</condition><then>
        <return>return;</return></then></if>

      <comment type="line">// output non-text node and get next node</comment>
      <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>output_node</name></decl>;</decl_stmt>
      <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;output_diff-&gt;back()-&gt;open_tags-&gt;back()-&gt;node-&gt;name</argument>, <argument>"diff:old"</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <expr_stmt><expr><name>output_node</name> <op:operator>=</op:operator> <name>diff_old_end</name></expr>;</expr_stmt></then>
      <else>else <if>if<condition>(<macro><name>strcmp</name><argument_list>(<argument>(const char *)rbuf-&gt;output_diff-&gt;back()-&gt;open_tags-&gt;back()-&gt;node-&gt;name</argument>, <argument>"diff:new"</argument>)</argument_list></macro> <expr><op:operator>==</op:operator> <lit:literal type="number">0</lit:literal></expr>)</condition><then>
        <expr_stmt><expr><name>output_node</name> <op:operator>=</op:operator> <name>diff_new_end</name></expr>;</expr_stmt></then>
      <else>else <block>{

        <expr_stmt><expr><name>output_node</name> <op:operator>=</op:operator> <name>rbuf</name><op:operator>-&gt;</op:operator><name>output_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>open_tags</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>node</name></expr>;</expr_stmt>
        <expr_stmt><expr><name>output_node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>=</op:operator> <op:operator>(</op:operator><name>xmlElementType</name><op:operator>)</op:operator><name>XML_READER_TYPE_END_ELEMENT</name></expr>;</expr_stmt>
      }</block></else></if></else></if>

      <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><name>output_node</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>output_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>COMMON</name></expr>)</condition><then> <block>{

        <comment type="line">//fprintf(stderr, "HERE OUTPUT COMMON\n");</comment>

        <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
        <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>output_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>COMMON</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      }</block></then>
      <else>else <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>output_diff</name><op:operator>-&gt;</op:operator><call><name>back</name><argument_list>()</argument_list></call><op:operator>-&gt;</op:operator><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then> <block>{

        <comment type="line">//fprintf(stderr, "HERE OUTPUT DELETE\n");</comment>
        <comment type="line">//fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node-&gt;name);</comment>

        <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>output_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>DELETE</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

      }</block></then> <else>else <block>{

        <comment type="line">//fprintf(stderr, "HERE OUTPUT INSERT\n");</comment>
        <comment type="line">//fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node-&gt;name);</comment>

        <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

        <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>output_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>INSERT</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
      }</block></else></if></else></if>

    }</block></while>

    <return>return;</return>
  }</block></then></if>

  <comment type="line">// output non-text node and get next node</comment>
  <expr_stmt><expr><call><name>outputNode</name><argument_list>(<argument><expr><op:operator>*</op:operator><name>node</name></expr></argument>, <argument><expr><name>writer</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  <if>if<condition>(<expr><name>operation</name> <op:operator>==</op:operator> <name>COMMON</name></expr>)</condition><then> <block>{

    <comment type="line">//fprintf(stderr, "HERE OUTPUT COMMON\n");</comment>
    <comment type="line">//fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node-&gt;name);</comment>

    <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
    <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>output_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  }</block></then>
  <else>else <if>if<condition>(<expr><name>operation</name> <op:operator>==</op:operator> <name>DELETE</name></expr>)</condition><then> <block>{

    <comment type="line">//fprintf(stderr, "HERE OUTPUT DELETE\n");</comment>
    <comment type="line">//fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node-&gt;name);</comment>

    <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_old</name><op:operator>-&gt;</op:operator><name>output_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

  }</block></then> <else>else <block>{

    <comment type="line">//fprintf(stderr, "HERE OUTPUT INSERT\n");</comment>
    <comment type="line">//fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node-&gt;name);</comment>

    <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>open_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>

    <expr_stmt><expr><call><name>update_diff_stack</name><argument_list>(<argument><expr><name>rbuf_new</name><op:operator>-&gt;</op:operator><name>output_diff</name></expr></argument>, <argument><expr><name>node</name></expr></argument>, <argument><expr><name>operation</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></else></if></else></if>

}</block></diff:old><diff:new><block>{

  <if>if<condition>(<expr><call><name>xmlTextReaderIsEmptyElement</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>)</condition><then>
    <return>return;</return></then></if>

  <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>node</name> =<init> <expr><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><name>indiff</name></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{

    <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then>
      <return>return;</return></then></if>

    <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>in_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
  }</block></then></if></else></if>
}</block></diff:new></function>

<function><diff:old><type><name>bool</name></type></diff:old><diff:new><type><name>void</name></type></diff:new> <diff:old><name>output_peek</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_old</name></decl></param>, <param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf_new</name></decl></param>, <param><decl><type><name>xmlNodePtr</name></type> <name>node</name></decl></param>, <param><decl><type><name>int</name></type> <name>operation</name></decl></param>, <param><decl><type><name>xmlTextWriterPtr</name></type> <name>writer</name></decl></param>)</parameter_list></diff:old><diff:new><name>update_issued_diff</name><parameter_list>(<param><decl><type>struct <name>reader_buffer</name> <type:modifier>*</type:modifier></type> <name>rbuf</name></decl></param>, <param><decl><type><name>xmlTextReaderPtr</name></type> <name>reader</name></decl></param>)</parameter_list></diff:new> <diff:old><block>{

  <comment type="line">//fprintf(stderr, "HERE PEAK\n");</comment>

  <return>return <expr><lit:literal type="boolean">true</lit:literal></expr>;</return>

}</block></diff:old><diff:new><block>{

  <if>if<condition>(<expr><call><name>xmlTextReaderIsEmptyElement</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr>)</condition><then>
    <return>return;</return></then></if>

  <decl_stmt><decl><type><name>xmlNodePtr</name></type> <name>node</name> =<init> <expr><call><name>getRealCurrentNode</name><argument_list>(<argument><expr><name>reader</name></expr></argument>)</argument_list></call></expr></init></decl>;</decl_stmt>
  <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_ELEMENT</name></expr>)</condition><then> <block>{

    <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>push_back</name><argument_list>(<argument><expr><lit:literal type="boolean">false</lit:literal></expr></argument>)</argument_list></call></expr>;</expr_stmt>
  }</block></then> <else>else <if>if<condition>(<expr><op:operator>(</op:operator><name>xmlReaderTypes</name><op:operator>)</op:operator><name>node</name><op:operator>-&gt;</op:operator><name>type</name> <op:operator>==</op:operator> <name>XML_READER_TYPE_END_ELEMENT</name></expr>)</condition><then> <block>{

    <if>if<condition>(<expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>size</name><argument_list>()</argument_list></call> <op:operator>==</op:operator> <lit:literal type="number">1</lit:literal></expr>)</condition><then>
      <return>return;</return></then></if>

    <expr_stmt><expr><name>rbuf</name><op:operator>-&gt;</op:operator><name>issued_diff</name><op:operator>-&gt;</op:operator><call><name>pop_back</name><argument_list>()</argument_list></call></expr>;</expr_stmt>
  }</block></then></if></else></if>
}</block></diff:new></function>
</unit>
