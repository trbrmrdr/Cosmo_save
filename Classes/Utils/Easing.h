#ifndef EASING_H
#define EASING_H

#ifdef EASING_USE_DBL_PRECIS
#define Float double
#else
#define Float float
#endif

namespace Easing {

	typedef Float(*AHEasingFunction)(Float);

	// Linear interpolation (no easing)
	Float LinearInterpolation(Float p);

	// Quadratic easing; p^2
	Float QuadraticEaseIn(Float p);
	Float QuadraticEaseOut(Float p);
	Float QuadraticEaseInOut(Float p);

	// Cubic easing; p^3
	Float CubicEaseIn(Float p);
	Float CubicEaseOut(Float p);
	Float CubicEaseInOut(Float p);

	// Quartic easing; p^4
	Float QuarticEaseIn(Float p);
	Float QuarticEaseOut(Float p);
	Float QuarticEaseInOut(Float p);

	// Quintic easing; p^5
	Float QuinticEaseIn(Float p);
	Float QuinticEaseOut(Float p);
	Float QuinticEaseInOut(Float p);

	// Sine wave easing; sin(p * PI/2)
	Float SineEaseIn(Float p);
	Float SineEaseOut(Float p);
	Float SineEaseInOut(Float p);

	// Circular easing; sqrt(1 - p^2)
	Float CircularEaseIn(Float p);
	Float CircularEaseOut(Float p);
	Float CircularEaseInOut(Float p);

	// Exponential easing, base 2
	Float ExponentialEaseIn(Float p);
	Float ExponentialEaseOut(Float p);
	Float ExponentialEaseInOut(Float p);

	// Exponentially-damped sine wave easing
	Float ElasticEaseIn(Float p);
	Float ElasticEaseOut(Float p);
	Float ElasticEaseInOut(Float p);

	// Overshooting cubic easing; 
	Float BackEaseIn(Float p);
	Float BackEaseOut(Float p);
	Float BackEaseInOut(Float p);

	// Exponentially-decaying bounce easing
	Float BounceEaseIn(Float p);
	Float BounceEaseOut(Float p);
	Float BounceEaseInOut(Float p);

}

#endif