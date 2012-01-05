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

<xsl:output method="xml" encoding="UTF-8" omit-xml-declaration="yes" standalone="yes"/>

<xsl:template match="//src:unit[not(descendant::src:unit)]">

<xsl:copy-of select="./node()"/>

<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="@*|node()">

<xsl:apply-templates select="@*|node()"/>

</xsl:template>

</xsl:stylesheet>