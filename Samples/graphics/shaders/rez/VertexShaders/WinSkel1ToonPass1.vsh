; Toon shader for 1 blend weight; first pass
vs.1.1
;------------------------------------------------------------------------------
; Constants
;    c0-c3     = matWorld0
;    c16-c19   = matViewProj
;    c20       = 0,0,0,0
;    c21       = 1,1,1,1
;    c50       = light direction
;    c51       = diffuse color
;
; Vertex components (as specified in the vertex declaration)
;    v0        = position
;    v1        = blend weight
;    v2        = normal
;    v3        = tex coord0
;------------------------------------------------------------------------------

; inputs
dcl_position v0
dcl_blendweight v1
dcl_normal v2
dcl_texcoord v3

; Transform position for world matrices
m4x4 r0, v0, c0
m4x4 r1, v0, c4

; Move the blend weights into a temp register
mov r4, v1

; Apply the blend weights by linear interpolating the positions into r5
mul r5, r0, r4.x       ; pos = p0*w0
sub r4.w, c21.x, r4.x  ; w1 = 1 - w0
mad r5, r1, r4.w, r5   ; pos = p0*w0 + p1*(1 - w0)

; Transform to projection space
m4x4 oPos, r5, c16

; Do the lighting calculation
mov oD0, c51

; Transform the normal into view space
dp3 r1.x, v2, c0
dp3 r1.y, v2, c1
dp3 r1.z, v2, c2

; tu = vNormal dot vLight for the toon texture
dp3 oT0.x, -r1, c50

; Just copy the texture coordinates
mov oT1, v3
