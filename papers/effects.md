# Effects

TAF allows you to insert effects to the rendering pipeline of an object.
these effects are actually shaders that get executed on the graphics card.

adding an effect to an object also adds its uniforms as properties.
to access them, type "EffectName/UniformName".

## Passthrough

no effect.

### Creation Arguments

```
Passthrough
```

## BoxBlur, GaussianBlur

spatial blur effect.

### Creation Arguments

in either case, Radius can be a double, or a list of 2 doubles (for X/Y).

- BoxBlur

```
BoxBlur Radius [Passes] [BorderTreatment]
```

- GaussianBlur

```
GaussianBlur Radius [BorderTreatment]
```

BorderTreatment can be:
- empty: out-of-bounds is transparent.
- bound: out-of-bounds is the previous pixel.
- wrap: out-of-bounds wraps.
