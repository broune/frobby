/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#ifndef NAME_FACTORY_GUARD
#define NAME_FACTORY_GUARD

#include <vector>
#include <string>

/** A NameFactory takes a name and then creates an instance of a class
 that has been previously registered under that name. This is done
 in a general way using templates.
*/
template<class AbstractProduct>
class NameFactory {
 public:
  typedef auto_ptr<AbstractProduct> (*FactoryFunction)();
  void registerProduct(const string& name, FactoryFunction function);

  /** Calls the function registered to the parameter name and returns
   the result. Returns null if name has not been registered. */
  auto_ptr<AbstractProduct> create(const string& name) const;

  /** Inserts into names all registered names that have the indicated
   prefix. */
  void getNamesWithPrefix(const string& prefix, vector<string>& names) const;

  /** Returns true if no names have been registered. */
  bool empty() const;

 private:
  typedef pair<string, FactoryFunction> Pair;
  typedef typename vector<Pair>::const_iterator const_iterator;
  vector<Pair> _pairs;
};

/** Registers the string returned by ConcreteProduct::getStaticName()
 to a function that default-constructs a ConcreteProduct.

 This is a utility function wrapping the registerProduct method of
 a NameFactory. It would make more sense as a member function, but
 some compilers have problems with template member functions. */
template<class ConcreteProduct, class AbstractProduct>
void nameFactoryRegister(NameFactory<AbstractProduct>& factory);



// These are implementations that have to be included here due
// to being templates.

template<class AbstractProduct>
auto_ptr<AbstractProduct> NameFactory<AbstractProduct>::
create(const string& name) const {
  for (const_iterator it = _pairs.begin(); it != _pairs.end(); ++it)
	if (it->first == name)
	  return it->second();
  return auto_ptr<AbstractProduct>();
}

template<class AbstractProduct>
void NameFactory<AbstractProduct>::
registerProduct(const string& name, FactoryFunction function) {
  _pairs.push_back(Pair(name, function));
}

template<class AbstractProduct>
void NameFactory<AbstractProduct>::
getNamesWithPrefix(const string& prefix, vector<string>& names) const {
  for (const_iterator it = _pairs.begin(); it != _pairs.end(); ++it)
	if (it->first.compare(0, prefix.size(), prefix) == 0)
	  names.push_back(it->first);
}

template<class AbstractProduct>
bool NameFactory<AbstractProduct>::empty() const {
  return _pairs.empty();
}

template<class ConcreteProduct, class AbstractProduct>
void nameFactoryRegister(NameFactory<AbstractProduct>& factory) {
  struct HoldsFunction {
	static auto_ptr<AbstractProduct> createConcreteProduct() {
	  return auto_ptr<AbstractProduct>(new ConcreteProduct());
	}
  };

  factory.registerProduct(ConcreteProduct::staticGetName(),
						  HoldsFunction::createConcreteProduct);
}

#endif
