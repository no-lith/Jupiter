Fonts Sample

This sample demonstrates the creation and display of vector (*.ttf) and bitmap (*.dtx) fonts.
It also demostrates using wchar true type fonts.

There are two variations of bitmap fonts: monospace and proportional. Each character of a 
monospace bitmap font is uniformly spaced.  With proportional fonts, each character can have 
its own spacing/alignment.  

A special note regarding proportional fonts: there are two methods to create proportional fonts.  
A font table is used to determine the extents of each letter in the bitmap (texture).  The font
table can either be user-supplied, or the engine can try to automatically generate one.
If the user wishes to have the engine create the font table, special preparations to the bitmap 
are required.  For each character in the bitmap, a green pixel (ARGB = 0X0000FF00) is marked at 
its lower right hand corner (for an example take a look at GE Centurion_14.dtx in the fonts 
resource folder).  

Font Table format:
Each character in the font is represented by 3 uint16 values in the font table.  The first uint16 
is the width of the character in pixels, the second and third uint16 are (u,v) texture coordinates 
of the character within the texture.

Controls:
B	: Cycle background color
G	: Toggle GUI display


Email questions to: support@touchdownentertainment.com
