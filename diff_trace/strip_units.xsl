<?xml version="1.0"?>

<xsl:stylesheet version="1.0"

xmlns="http://www.sdml.info/srcML/src"
xmlns:src="http://www.sdml.info/srcML/src"
xmlns:cpp="http://www.sdml.info/srcML/cpp"
xmlns:lit="http://www.sdml.info/srcML/literal"
xmlns:op="http://www.sdml.info/srcML/operator"
xmlns:type="http://www.sdml.info/srcML/modifier"
xmlns:diff="http://www.sdml.info/srcDiff"

xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="//src:unit[not(//src:unit)]">

<xsl:copy-of select="./"/>

</xsl:template>

</xsl:stylesheet>