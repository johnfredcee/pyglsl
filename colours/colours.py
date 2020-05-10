#!/usr/bin/env python
#
# colours graphics maths module
#
# Copyright (c) 2006 Alex Holkner <Alex.Holkner@mail.google.com>
# Copyright (c) 2011 Eugen Zagorodniy <https://github.com/ezag/>
# Copyright (c) 2011 Dov Grobgeld <https://github.com/dov>
# Copyright (c) 2012 Lorenzo Riano <https://github.com/lorenzoriano>
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation; either version 2.1 of the License, or (at your
# option) any later version.
# 
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
# for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

import math
import operator
import types

try:
    long
except NameError:
    long = int


# Some magic here.  If _use_slots is True, the classes will derive from
# object and will define a __slots__ class variable.  If _use_slots is
# False, classes will be old-style and will not define __slots__.
#
# _use_slots = True:   Memory efficient, probably faster in future versions
#                      of Python, "better".
# _use_slots = False:  Ordinary classes, much faster than slots in current
#                      versions of Python (2.4 and 2.5).
_use_slots = True

# Implement _use_slots magic.
class _ColorsMetaclass(type):
    def __new__(cls, name, bases, dct):
        if '__slots__' in dct:
            dct['__getstate__'] = cls._create_getstate(dct['__slots__'])
            dct['__setstate__'] = cls._create_setstate(dct['__slots__'])
        if _use_slots:
            return type.__new__(cls, name, bases + (object,), dct)
        else:
            if '__slots__' in dct:
                del dct['__slots__']
            return types.ClassType.__new__(types.ClassType, name, bases, dct)

    @classmethod
    def _create_getstate(cls, slots):
        def __getstate__(self):
            d = {}
            for slot in slots:
                d[slot] = getattr(self, slot)
            return d
        return __getstate__

    @classmethod
    def _create_setstate(cls, slots):
        def __setstate__(self, state):
            for name, value in state.items():
                setattr(self, name, value)
        return __setstate__

__metaclass__ = _ColorsMetaclass


class RGB:
    __slots__ = ['r', 'g', 'b']
    __hash__ = None

    def __init__(self, r=0.0, g=0.0, b=0.0):
        self.r = r
        self.g = g
        self.b = b

    def __copy__(self):
        return self.__class__(self.r, self.g, self.b)

    copy = __copy__

    def __repr__(self):
        return 'RGB(%.2f, %.2f, %.2f)' % (self.r,
                                           self.g,
                                           self.b)

    def __eq__(self, other):
        if isinstance(other, RGB):
            return self.r == other.r and \
                   self.g == other.g and \
                   self.b == other.b
        else:
            assert hasattr(other, '__len__') and len(other) == 3
            return self.r == other[0] and \
                   self.g == other[1] and \
                   self.b == other[2]

    def __ne__(self, other):
        return not self.__eq__(other)

    def __nonzero__(self):
        return bool(self.r != 0 or self.g != 0 or self.b != 0)

    def __len__(self):
        return 3

    def __getitem__(self, key):
        return (self.r, self.g, self.b)[key]

    def __setitem__(self, key, value):
        l = [self.r, self.g, self.b]
        l[key] = value
        self.r, self.g, self.b = l

    def __iter__(self):
        return iter((self.r, self.g, self.b))

    def __getattr__(self, name):
        try:
            return tuple([(self.r, self.g, self.b)['rgb'.index(c)] \
                          for c in name])
        except ValueError:
            raise AttributeError(name)


    def __add__(self, other):
        if isinstance(other, RGB):
            return RGB(self.r + other.r,
                        self.g + other.g,
                        self.b + other.b)
        else:
            assert hasattr(other, '__len__') and len(other) == 3
            return RGB(self.r + other[0],
                        self.g + other[1],
                        self.b + other[2])
    __radd__ = __add__

    def __iadd__(self, other):
        if isinstance(other, RGB):
            self.r += other.r
            self.g += other.g
            self.b += other.b
        else:
            self.r += other[0]
            self.g += other[1]
            self.b += other[2]
        return self

    def __sub__(self, other):
        if isinstance(other, RGB):
            return RGB(self.r - other.r,
                        self.g - other.g,
                        self.b - other.b)
        else:
            assert hasattr(other, '__len__') and len(other) == 3
            return RGB(self.r - other[0],
                        self.g - other[1],
                        self.b - other[2])

   
    def __rsub__(self, other):
        if isinstance(other, RGB):
            return RGB(other.r - self.r,
                        other.g - self.g,
                        other.b - self.b)
        else:
            assert hasattr(other, '__len__') and len(other) == 3
            return RGB(other.r - self[0],
                        other.g - self[1],
                        other.b - self[2])

    def __mul__(self, other):
        if isinstance(other, RGB):
            return RGB(self.r * other.r,
                       self.g * other.g,
                       self.b * other.b)
        else: 
            assert type(other) in (int, long, float)
            return RGB(self.r * other,
                        self.g * other,
                        self.b * other)

    __rmul__ = __mul__

    def __imul__(self, other):
        assert type(other) in (int, long, float)
        self.r *= other
        self.g *= other
        self.b *= other
        return self

    def __div__(self, other):
        assert type(other) in (int, long, float)
        return RGB(operator.div(self.r, other),
                    operator.div(self.g, other),
                    operator.div(self.b, other))


    def __rdiv__(self, other):
        assert type(other) in (int, long, float)
        return RGB(operator.div(other, self.r),
                       operator.div(other, self.g),
                       operator.div(other, self.b))

    def __floordiv__(self, other):
        assert type(other) in (int, long, float)
        return RGB(operator.floordiv(self.r, other),
                    operator.floordiv(self.g, other),
                    operator.floordiv(self.b, other))


    def __rfloordiv__(self, other):
        assert type(other) in (int, long, float)
        return RGB(operator.floordiv(other, self.r),
                    operator.floordiv(other, self.g),
                    operator.floordiv(other, self.b))

    def __truediv__(self, other):
        assert type(other) in (int, long, float)
        return RGB(operator.truediv(self.r, other),
                    operator.truediv(self.g, other),
                    operator.truediv(self.b, other))


    def __rtruediv__(self, other):
        assert type(other) in (int, long, float)
        return RGB(operator.truediv(other, self.r),
                    operator.truediv(other, self.g),
                    operator.truediv(other, self.b))
 

class RGBA:
    __slots__ = ['r', 'g', 'b', 'a']
    __hash__ = None

    def __init__(self, r=0.0, g=0.0, b=0.0, a=1.0):
        self.r = r
        self.g = g
        self.b = b
        self.a = a

    def __copy__(self):
        return self.__class__(self.r, self.g, self.b, self.a)

    copy = __copy__

    def __repr__(self):
        return 'RGBA(%.2f, %.2f, %.2f, %2f)' % (self.r, self.g, self.b, self.a)

    def __eq__(self, other):
        if isinstance(other, RGBA):
            return self.r == other.r and \
                   self.g == other.g and \
                   self.b == other.b and \
                   self.b == other.a 
        else:
            assert hasattr(other, '__len__') and len(other) == 4
            return self.r == other[0] and \
                   self.g == other[1] and \
                   self.b == other[2] and \
                   self.a == other[3]

    def __ne__(self, other):
        return not self.__eq__(other)

    def __nonzero__(self):
        return bool(self.r != 0 or self.g != 0 or self.b != 0 or self.a !=0)

    def __len__(self):
        return 4

    def __getitem__(self, key):
        return (self.r, self.g, self.b, self.a)[key]

    def __setitem__(self, key, value):
        l = [self.r, self.g, self.b, self.a]
        l[key] = value
        self.r, self.g, self.b, self.a = l

    def __iter__(self):
        return iter((self.r, self.g, self.b, self.a))

    def __getattr__(self, name):
        try:
            return tuple([(self.r, self.g, self.b, self.a)['rgba'.index(c)] \
                          for c in name])
        except ValueError:
            raise AttributeError(name)


    def __add__(self, other):
        if isinstance(other, RGBA):
            return RGBA(self.r + other.r,
                        self.g + other.g,
                        self.b + other.b,
                        self.a + self.a)
        else:
            assert hasattr(other, '__len__') and len(other) == 4
            return RGBA(self.r + other[0],
                        self.g + other[1],
                        self.b + other[2],
                        self.a + other[3])
    __radd__ = __add__

    def __iadd__(self, other):
        if isinstance(other, RGBA):
            self.r += other.r
            self.g += other.g
            self.b += other.b,
            self.a += other.a
        else:
            self.r += other[0]
            self.g += other[1]
            self.b += other[2]
            self.a += other[3]
        return self

    def __sub__(self, other):
        if isinstance(other, RGBA):
            return RGBA(self.r - other.r,
                        self.g - other.g,
                        self.b - other.b,
                        self.a - other.a)
        else:
            assert hasattr(other, '__len__') and len(other) == 4
            return RGBA(self.r - other[0],
                        self.g - other[1],
                        self.b - other[2],
                        self.a - other[3])

   
    def __rsub__(self, other):
        if isinstance(other, RGBA):
            return RGBA(other.r - self.r,
                        other.g - self.g,
                        other.b - self.b,
                        other.a - self.a)
        else:
            assert hasattr(other, '__len__') and len(other) == 4
            return RGBA(other.r - self[0],
                        other.g - self[1],
                        other.b - self[2],
                        other.a - self[3])

    def __mul__(self, other):
        if isinstance(other, RGBA):
            return RGBA(self.r * other.r,
                       self.g * other.g,
                       self.b * other.b,
                       self.a * other.a)
        else: 
            assert type(other) in (int, long, float)
            return RGBA(self.r * other,
                        self.g * other,
                        self.b * other,
                        self.a * other)

    __rmul__ = __mul__

    def __imul__(self, other):
        assert type(other) in (int, long, float)
        self.r *= other
        self.g *= other
        self.b *= other
        self.a *= other
        return self

    def __div__(self, other):
        assert type(other) in (int, long, float)
        return RGBA(operator.div(self.r, other),
                    operator.div(self.g, other),
                    operator.div(self.b, other),
                    operator.div(self.a, other))


    def __rdiv__(self, other):
        assert type(other) in (int, long, float)
        return RGBA(operator.div(other, self.r),
                       operator.div(other, self.g),
                       operator.div(other, self.b),
                       operator.div(other, self.a))
    def __floordiv__(self, other):
        assert type(other) in (int, long, float)
        return RGBA(operator.floordiv(self.r, other),
                    operator.floordiv(self.g, other),
                    operator.floordiv(self.b, other),
                    operator.floordiv(self.a, other))


    def __rfloordiv__(self, other):
        assert type(other) in (int, long, float)
        return RGBA(operator.floordiv(other, self.r),
                    operator.floordiv(other, self.g),
                    operator.floordiv(other, self.b),
                    operator.floordiv(other, self.a))

    def __truediv__(self, other):
        assert type(other) in (int, long, float)
        return RGBA(operator.truediv(self.r, other),
                    operator.truediv(self.g, other),
                    operator.truediv(self.b, other),
                    operator.truediv(self.a, other))


    def __rtruediv__(self, other):
        assert type(other) in (int, long, float)
        return RGBA(operator.truediv(other, self.r),
                    operator.truediv(other, self.g),
                    operator.truediv(other, self.b),
                    operator.truediv(other, self.a))

if __name__ == "__main__":
    c0 = RGB(0.5, 0.1, 0.2)
    c1 = RGBA(00.5, 0.1, 0.2, 0.0)
