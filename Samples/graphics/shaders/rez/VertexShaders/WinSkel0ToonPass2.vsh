; Toon shader; second pass
vs.1.1
;------------------------------------------------------------------------------
; Constants
;    c0-c3     = matWorld0
;    c16-c19   = matViewProj
;    c40       = scale factor
;    c50       = light direction
;    c51       = diffuse color
;
; Vertex components (as specified in the vertex DECL)
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
