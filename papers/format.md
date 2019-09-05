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

### end

explicitly mark end of animation.

```
1 end
```
