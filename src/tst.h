#ifndef TST_H_
#define TST_H_

#include <iterator>
#include <memory>
#include <algorithm>
#include <functional>
#include <vector>
#include <exception>
#include <utility>
#include <string>

namespace Detail {
    // Funkcja fold na zakresie wyznaczonym przez iteratory działa następująco:
    // functor(...functor(functor(acc, *first), *(first + 1))..., *(last - 1))
    // W szczególnym przypadku first == last fold zwraca acc.
    template<typename Iter, typename Acc, typename Functor>
    Acc fold(Iter first, Iter last, Acc acc, Functor functor) {
        return first == last ? acc :
               fold(std::next(first), last, functor(acc, *first), functor);
    }
}

// Klasa niezmienialnych drzew wyszukiwań ternarnych.
template<typename C = char>
class TST {
private:
    // Klasa reprezentująca wierzchołek drzewa, przechowująca pojedyńczą
    // literę oraz znacznik końca słowa.
    struct Node {
        // Zawartość wierzchołka.
        const C _value;
        const bool _word;

        // Bezpośredni potomkowie.
        const std::shared_ptr<Node> _left;
        const std::shared_ptr<Node> _center;
        const std::shared_ptr<Node> _right;

        // Inicjuje wszystkie pola.
        Node(C value, bool word, std::shared_ptr<Node> left,
             std::shared_ptr<Node> center, std::shared_ptr<Node> right)
                : _value(value), _word(word), _left(left), _center(center), _right(right) {}
    };

    // Korzeń drzewa.
    const std::shared_ptr<Node> _root;

    // Utożsamia korzeń z drzewem.
    TST(std::shared_ptr<Node> node)
            : _root(node) {}

    // Bezpośrednio inicjuje korzeń.
    TST(C value, bool word, TST left, TST center, TST right)
            : _root(std::make_shared<Node>(value, word, left._root, center._root, right._root)) {}

    // Zapewnia niepustość drzewa.
    void assert_not_empty() const {
        if (empty()) {
            throw std::logic_error("Ternary search tree empty.");
        }
    }

    // Wyszukuje wierzchołek odpowiadający najdłuższemu wspólnemu
    // prefiksowi pewnego słowa w drzewie oraz napisu @str.
    // Zwraca również długość tego prefiksu.
    std::pair<size_t, TST> prefix_search(const C *str, size_t acc, TST parent) const {
        return empty() || !*str ? std::pair(acc, parent) :
               value() > *str ? left().prefix_search(str, acc, *this) :
               value() < *str ? right().prefix_search(str, acc, *this) :
               center().prefix_search(str + 1, acc + 1, *this);
    }

public:
    TST() = default;

    // Inicjuje drzewo jako ścieżkę.
    TST(const std::basic_string<C> &str)
            : TST(str.c_str()) {}

    TST(const C *str)
            : TST(!*str ? TST() : TST(*str, !*(str + 1), TST(), TST(str + 1), TST())) {}

    // Dodaje napis @str do drzewa (niezmieniając tego drzewa).
    // Efekt uboczny zostaje przekazany jako wynik.
    TST operator+(const std::basic_string<C> &str) const {
        return operator+(str.c_str());
    }

    TST operator+(const C *str) const {
        return *str == '\0' ? *this :
               empty() ? TST(str) :
               *str > value() ? TST(value(), word(), left(), center(), right() + str) :
               *str < value() ? TST(value(), word(), left() + str, center(), right()) :
               TST(*str, word() || !*(str + 1), left(), center() + (str + 1), right());
    }

    // Zwraca literę w korzeniu drzewa, lub wyrzuca wyjątek
    // `std::logic_error` gdy drzewo jest puste.
    C value() const {
        assert_not_empty();
        return _root->_value;
    }

    // Zwraca znacznik końca słowa w korzeniu drzewa lub wyrzuca
    // wyjątek `std::logic_error` gdy drzewo jest puste.
    bool word() const {
        assert_not_empty();
        return _root->_word;
    }

    // Zwraca lewego syna korzenia w drzewie lub wyrzuca
    // wyjątek `std::logic_error` gdy drzewo jest puste.
    TST left() const {
        assert_not_empty();
        return _root->_left;
    }

    // Zwraca środkowego syna korzenia w drzewie lub wyrzuca
    // wyjątek `std::logic_error` gdy drzewo jest puste.
    TST center() const {
        assert_not_empty();
        return _root->_center;
    }

    // Zwraca prawego syna korzenia w drzewie lub wyrzuca
    // wyjątek `std::logic_error` gdy drzewo jest puste.
    TST right() const {
        assert_not_empty();
        return _root->_right;
    }

    // Zwraca #true wtw. drzewo jest puste.
    bool empty() const {
        return _root == nullptr;
    }

    // Zwraca #true wtw. drzewo zawiera napis @str.
    bool exist(const std::basic_string<C> &str) const {
        auto[length, node] = prefix_search(str.c_str(), 0, TST());
        return length == str.length() && !node.empty() && node.word();
    }

    // Wyszukuje najdłuższy wspólny prefiks słowa str i słów zawartych w danym
    // drzewie. Przykład: jeśli tst składa się ze słów "category", "functor"
    // oraz "theory" to tst.prefix("catamorphism") daje rezultat "cat".
    std::basic_string<C> prefix(const std::basic_string<C> &str) const {
        return str.substr(0, prefix_search(str.c_str(), 0, TST()).first);
    }

    // Wykonuje redukcję drzewa za pomocą @functor : Acc x TST -> Acc,
    // przechodząc drzewo w kolejności inorder.
    template<typename Acc, typename Functor>
    Acc fold(Acc acc, Functor functor) const {
        if (empty()) {
            return acc;
        }

        std::vector<TST> children{_root->_left, _root->_center, _root->_right};
        auto fold_bind = [&](Acc a, const TST &t) { return t.fold(a, functor); };
        Acc reduced = Detail::fold(children.begin(), children.end(), acc, fold_bind);

        return functor(reduced, *this);
    }

    // Zwraca liczbę wierzchołków w drzewie.
    size_t size() const {
        return this->fold(size_t(0), [](size_t acc, const TST &) {
            return acc + 1;
        });
    }
};

#endif  // TST_H_
