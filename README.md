# tildearrow Animation Framework (TAF)

this is my own video creation and compositing tool which I began working on mid-August 2019, and made it public in early September.

at this moment there isn't much to it, and it has some flaws.

## background

back in 2015 I discovered open-source video editing for the first time, in the form of OpenShot. I didn't get used to it, because it was too simple and didn't fit my needs.

then, after finding out about Kdenlive, I decided to switch to that. it worked great (pretty much), until in mid-2016 I hit a massive bug: it can break your project.
there is a 1/10 chance that doing something in the timeline will cause it to desync with MLT's internal timeline, causing issues such as ghost clips (they appear in the player, and you can't move a clip to a region where there's one) (and even worse, if you try to delete the source of said ghost clip, it crashes).
the only way to work around this issue is to save almost every time after doing something, and when you see a desync, close Kdenlive and re-open it.

but the truth is, I don't know why did they have to manage 2 timelines and keep them in sync! quoting my Kdenlive rant:

> a freaking video editor shouldn't keep 2 timelines!!! 
> it should just have 1 timeline sync'ed to both the GUI, and the renderer (MLT)...

so I began finding new solutions that would fare much better than Kdenlive, and yep, found one: Blender.

however, Blender has 2 issues:

- the VSE is pretty much post-processing only. you can't do compositing and therefore no "pro-grade" effects such as motion blur.
- even if you use a script to turn Blender into a node-based compositor, it is too slow to be humanly operable. seriously, I mean, playing a video within Blender will cause it to slow down so much you can't even watch it in real-time.

so I went back to Kdenlive for a time, but then I hit a point, in where my needs (e.g. motion blur) were simply not fulfilled by Kdenlive...

...thankfully, there was Natron to the rescue! (which I began to use in 2019)

however, let's be honest. Natron became useless for any serious compositing work, which I tried to do a few months ago. you wanna know why?

well, it's because:

- it is too slow. yeah, very slow. despite claiming to do GPU acceleration, none of the base plugins do so.
- sometimes it can crash if you open the curve editor due to an X error.
- often, **the rendering job will get stuck**, which is simply not tolerable. is this a joke?

so, I said, screw it! if nothing works for me, and the other solutions are unaffordable at the moment, I am going to do the insane: build a whole video editor/compositor from scratch.

and so TAF was born.
