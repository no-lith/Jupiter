; Toon shader; first pass
vs.1.1
;------------------------------------------------------------------------------
; Constants
;    c0-c3     = matWorld0
;    c16-c19   = matViewProj
;    c50       = light direction
;    c51       = diffuse color
;
; Vertex components (as specified in the vertex declaration)
;    v0        = position
;    v1        = normal
;    v2        = tex coord0
;------------------------------------------------------------------------------

; inputs
dcl_position v0
dcl_normal v1
dcl_texcoord v2

; Transform position for world matrix
m4x4 r0, v0, c0

; Transform to projection space
m4x4 oPos, r0, c16

; Do the lighting calculation
mov oD0, c51

; Transform the normal into view space
dp3 r1.x, v1, c0
dp3 r1.y, v1, c1
dp3 r1.z, v1, c2

; tu = vNormal dot vLight for the toon texture
dp3 oT0.x, -r1, c50

; Just copy the texture coordinates
mov oT1, v2
