#ifndef MODELDATAADAPTER_H
#define MODELDATAADAPTER_H

#include <QMap>
#include <QVariant>
#include <functional>

namespace uiEx {

template <class Value> class ModelDataAdapter
{
public:
    typedef Value value_type;
    typedef std::function<QVariant(const Value&)> GetterType;
    typedef std::function<bool(Value&, const QVariant&)> SetterType;

    void addGetter(int column, int role)
    {
        columnGetters[column].insert(role, [](const Value & v) { return v; });
        columnManipulatorAdded(column);
    }

    template <class R>
    void addGetter(int column, int role, R(Value::*m)() const)
    {
        columnGetters[column].insert(role, std::mem_fn(m));
        columnManipulatorAdded(column);
    }

    template <class T> void addGetter(int column, int role, T Value::*m)
    {
        columnGetters[column].insert(role, std::mem_fn(m));
        columnManipulatorAdded(column);
    }

    template <class Function>
    void addGetter(int column, int role, const Function& function)
    {
        columnGetters[column].insert(role, [function](const Value & value)
        {
            return QVariant(function(value));
        });
        columnManipulatorAdded(column);
    }

    template <class Function>
    void addSetter(int column, int role, const Function& setter)
    {
        columnSetters[column].insert(role, setter);
    }

    QVariant data(const Value& value, int column, int role) const
    {
        if (columnGetters.contains(column) &&
            columnGetters[column].contains(role))
        {
            return columnGetters[column][role](value);
        }

        return QVariant();
    }

    bool setData(Value* value, int column, const QVariant& data, int role)
    {
        if (!columnSetters.contains(column) ||
            !columnSetters[column].contains(role))
        {
            return false;
        }

        return columnSetters[column][role](*value, data);
    }

    Qt::ItemFlags getFlags(int column) const
    {
        if (columnSetters.contains(column))
        {
            return Qt::ItemIsEnabled | Qt::ItemIsEditable;
        }

        return Qt::ItemIsEnabled;
    }

protected:
    virtual void columnManipulatorAdded(int column) = 0;

private:
    // role to getter
    typedef QMap<int, GetterType> RoleGetters;
    typedef QMap<int, SetterType> RoleSetters;

    // column number to roles
    QMap<int, RoleGetters> columnGetters;
    QMap<int, RoleSetters> columnSetters;
};


}


#endif // MODELDATAADAPTER_H
