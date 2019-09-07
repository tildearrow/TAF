# TAF format details (tildearrow animation file)

the basic syntax resembles BASIC, except for the beginning.

```
Frame Command
```

e.g.

```
10 rem Test
```

## header

```
---TAF PROJECT BEGIN---
0 identify "Name" "Author"
0 canvas Width Height
0 rate SampleRate OutFrameRate
0 length Begin End
```

## commands

### rem

comment. the line is ignored.

```
1 rem Comment
```

### insert

insert an object.

```
1 insert ObjectType Name X Y [args...]
```

see [objectTypes.md](objectTypes.md) for more information.

### prop

set object property.

```
1 prop Object Name Value
```

### move

shorthand for `prop Object pos [X Y]`.

```
1 move Object X Y
```

### animate

begin animation.

to animate sub-props, use the syntax `Property.Index`.

```
1 animate Object Property [Frame Value Slope ...]
```

#### Index Aliases

i | aliases
--|-----------
0 | x, r
1 | y, g
2 | z, b
3 | w, a

### pipeline

create an effect pipeline.

### effect

insert effect/process to pipeline.

```
1 effect Pipeline Effect [args...]
```

see [effects.md](effects.md) for more information.

### attach

attach pipeline to an object or pipeline.
you can attach multiple pipelines to an object.

```
1 attach Object Pipeline Stage
```
```
1 attach Pipeline Pipeline Stage BlendMode
```

`Stage` may be one of the following:

- pre: before blend color multiplication
- mid: before motion blur
- post: after all stock processes

`BlendMode` may be one of the following:

- add: addition
- sub: subtraction
- avg: average
- mult: multiplication/modulation
- blend: blending
- mask: alpha masking
- invMask: inverse alpha masking

### end

explicitly mark end of animation.

```
1 end
```
