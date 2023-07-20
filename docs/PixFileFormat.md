Pix file format
===============

It is [RIFF derived](https://en.wikipedia.org/wiki/Resource_Interchange_File_Format). 
It has "RIFF" on bytes 0 to 3 and  "PIX " on 8 to 11 (Byte eleven is a space '0x20').
It also has a tag "FMT " that is structured as follows:

<table border>
  <tr>
    <th>0</th>
    <th>1</th>
    <th>2</th>
    <th>3</th>
    <th>4</th>
    <th>5</th>
    <th>6</th>
    <th>7</th>
    <th>8</th>
    <th>9</th>
    <th>10</th>
    <th>11</th>
    <th>12</th>
    <th>13</th>
    <th>14</th>
    <th>15</th>
    <th>16</th>
    <th>17</th>
    <th>18</th>
    <th>19</th>
  </tr>
<tr>
  <td colspan=4 align=center>'FMT '</td>
  <td colspan=4 align=center>8 or 12</td>
  <td colspan=2 align=center>width</td>
  <td colspan=2 align=center>height</td>
  <td colspan=4 align=center title="SDL_PixelFormat">format</td>
  <td colspan=4 align=center title="SDL_PixelFormat"><i>variant</i></td>
</tr>
<table>

Variant | Tags                  | Meaning 
------- | --------------------- | -------
0       | All tags              | Bleeding edge
1       | FMT, DATA, CKEY, PALT | Still image without layers (default if no variant field present)
3       | _1_ + LAYR, LOPT      | Still image with layers
5       | _1_ + FRME, FPS       | Animated image without layers
7       | _3_ + _5_             | Animated image with layers

Variants 1 to 4
---------------

Tag     | Description
------- | --------------
PIX     | Format tag, only appears once
FMT     | Format WW HH bV where b is bits per pixel and V is variant, 0 means edge (most recent) otherwise use 1 bits must be 1, 4, 8, 16, 24, 32. where, <= 8 means grayscale, other wise is A?BGR, with 16 being with B5G6R5
DATA    | The pixel data. Its size must be exactly: `WW*HH*ceil(b/8)`.
CKEY    | The color key, in the pixel format
PALT    | SZ = 4*n, where 2 <= n <= 256. This is the palette, as sequence of unsigned int4 ABGR. This is optional
LAYR    | SZ >=2 First 2 bytes are the Layer order, unsigned (larger numbers tops smaller ones) and the remainder is layer name, if applicable
LOPT    | SZ = 1, bit 1 controls visibility, bit 2 controls alpha
FPS     | SZ =2 Must be declared before first FRME, This is the desired frame rate multiplied by 360. SO 360 is 1 FPS 60*360 is 60 FPS and so on
FRME    | SZ >=4 RRDD, Where RR are 2 bytes with order, like in LAYR and DD is duration of the frame, in multiples of FPS. The remainder is the name, if applicable
RMLR    | SZ = 2 Remove layer
RMFR    | SZ = 2 Remove frame
CHKP    | SZ = 2 History checkpoint. bit 1 means current, bit 2 means synced with disc (show as not dirty)
