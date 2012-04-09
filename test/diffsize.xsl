<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:src="http://www.sdml.info/srcML/src"
    xmlns:cpp="http://www.sdml.info/srcML/cpp"
    xmlns:diff="http://www.sdml.info/srcDiff"
    xmlns:str="http://exslt.org/strings"
        extension-element-prefixes="str"
        version="1.0">
<!--
itervar.xsl Fixes "iterator-variables are local"

Michael L. Collard collard@cs.kent.edu
-->
<xsl:output method="text" encoding="string" omit-xml-declaration="yes" standalone="yes" />

<xsl:template match="node()"><xsl:apply-templates/></xsl:template>

<xsl:template match="diff:insert | diff:delete">

  <xsl:value-of select="count(.//node()[not(self::diff:*) and ancestor::diff:*[1][not(diff:common)]])"/>
<xsl:text>
</xsl:text>

</xsl:template>

</xsl:stylesheet>
