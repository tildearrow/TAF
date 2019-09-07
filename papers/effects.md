# Effects

TAF allows you to insert effects to the rendering pipeline of an object.
these effects are actually shaders that get executed on the graphics card.

adding an effect to an object also adds its uniforms as properties.
to access them, type "EffectName/UniformName".

## BoxBlur, GaussianBlur

spatial blur effect.

### Creation Arguments

- BoxBlur

```
1 effect Object Stage BoxBlur Passes
```

## Passthrough
