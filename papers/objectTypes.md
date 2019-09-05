# Object Types

TAF currently implements 6 object types, which will be discussed below.

## Properties for all Object types

name      |  type  | description
----------|--------|---------------------------------------
name      | string | object name
pos       | coords | position
scale     | coords | scale, of course.
origin    | coords | origin, between 0.0 and 1.0
rot       | double | rotation (deg)
blend     | color  | color to multiply the object with (includes alpha)
depth     |  int   | drawing order
life      |  int   | lifetime of the object (in frames). after this time, the object will be destroyed.

## Sprite

a Sprite is a stationary image. this means, if you load a video, it will only display the first frame.

### Creation Arguments

```
0 insert Sprite Name X Y file [crop1 crop2]
```

### Properties

name      |  type  | description
----------|--------|---------------------------------------
file      | string | path to image
crop1     | coords | top left corner crop
crop2     | coords | bottom right corner crop

## MotionSprite

a MotionSprite is the same as a Sprite, but it is oriented towards video.

### Creation Arguments

```
0 insert MotionSprite Name X Y file begin end [speed] [crop1 crop2]
```

### Properties

name      |  type  | description
----------|--------|---------------------------------------
file      | string | path to image
crop1     | coords | top left corner crop
crop2     | coords | bottom right corner crop
begin     |  int   | first frame
end       |  int   | last frame
onError   |  bool  | freeze on error if true, blank if false
cutBefore |  bool  | blank before begin. if not, then freeze
cutAfter  |  bool  | blank after end. if not, then freeze
speed     | double | video speed
seek      | double | current frame

## Text

a Text object display text rendered in real-time using FreeType.

### Creation Arguments

```
0 insert Text Name X Y font fontSize [text]
```

### Properties

name       |  type  | description
-----------|--------|---------------------------------------
font       | string | path to font
text       | string | text to display
textColor  | color  | text color
fontSize   | double | font size
lineHeight | double | extra line height
charSep    | double | extra character separation
outline    | double | outline width
outColor   | color  | outline color

## Rotoscope

a Rotoscope is the same as a MotionSprite, but frames are loaded individually and it offers greater control over which frames are displayed.

### Creation Arguments

### Properties

## Particle

a Particle is an object you are not supposed to normally use. please use PartSys to generate them instead.

### Creation Arguments

### Properties

## PartSys

a PartSys is a particle system. it generates Particle-type objects.

### Creation Arguments

### Properties

## AudioTrack

an AudioTrack object is a special object: it does not produce any video output, and many of the global properties have different meanings.

### Creation Arguments

```
0 insert AudioTrack Name LRPan FBPan file
```

### Properties

name      |  type  | description
----------|--------|---------------------------------------
pos       | coords | panning within a circle
scale     | coords | channel separation
origin    | coords | center of panning
rot       | double | channel rotation
blend     | color  | the alpha component is volume
file      | string | path to audio
begin     |  int   | first sample
end       |  int   | last sample
speed     | double | audio speed
seek      |  int   | current sample
