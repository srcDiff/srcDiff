<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:src="http://www.sdml.info/srcML/src"
    xmlns:cpp="http://www.sdml.info/srcML/cpp"
    xmlns:diff="http://www.sdml.info/srcDiff"
        version="1.0">
<!--
    diffonly.xsl

    Removes all srcML tags (except for src:unit), but leaves
    diff tags and text.

    Michael L. Collard
    collard@uakron.edu
-->

<xsl:template match="src:*"><xsl:apply-templates/></xsl:template>

<xsl:template match="src:unit|diff:*"><xsl:copy><xsl:apply-templates/></xsl:copy></xsl:template>

</xsl:stylesheet>
