; Toon shader for 3 blend weights; first pass
vs.1.1
;------------------------------------------------------------------------------
;    c0-c3     = matWorld0
;    c4-c7     = matWorld1
;    c8-c11    = matWorld2
;    c12-c15   = matWorld3
;    c16-c19   = matViewProj
;    c20       = 0,0,0,0
;    c21       = 1,1,1,1
;    c50       = light direction
;    c51       = diffuse color
;
; Vertex components (as specified in the vertex declaration)
;    v0        = position
;    v1        = blend weights
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
m4x4 r2, v0, c8
m4x4 r3, v0, c12

; Move the blend weights into a temp register
mov r4, v1

; Apply the blend weights by linear interpolating the positions into r5
mul r5, r0, r4.x       ; pos = p0*w0
mad r5, r1, r4.y, r5   ; pos = p0*w0 + p1*w1
mad r5, r2, r4.z, r5   ; pos = p0*w0 + p1*w1 + p2*w2
add r4.w, r4.x, r4.y   ; w4 = w0 + w1
add r4.w, r4.w, r4.z   ; w4 = w0 + w1 + w2
sub r4.w, c21.x, r4.w  ; w4 = 1 - (w0 + w1 + w2)
mad r5, r3, r4.w, r5   ; pos = p0*w0 + p1*w1 + p2*w2 + p3*(1 - (w0 + w1 + w2))

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
