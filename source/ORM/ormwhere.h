#ifndef ORMWHERE_H
#define ORMWHERE_H

#include <QVariant>
#include <QString>

/*!
   \class ORMWhere
   \brief The ORMWhere class use for generate \a WHERE expressions.

   You must create object and pass three parameters: \a fieldName, \a condition and \a value.
   Example:
   \code
   ORMWhere where("name", ORMWhere::Equals, "Bob");
   \endcode
   For this class defines && and || operators, you can use next syntaxis:
   \code
   ...
   model.where(ORMWhere("name", ORMWhere::Equals, "Bob") || ORMWhere("name", ORMWhere::Equals, "Paul"));
   ...
   \endcode
 */

class ORMWhere
{
public:
    /*!
       \enum ORMWhere::Condition
       \brief Enumeration of type of comparisons
       \var ORMWhere::Condition ORMWhere::Equals
       Equivalent for \a =
       \var ORMWhere::Condition ORMWhere::NotEquals
       Equivalent for \a !=
       \var ORMWhere::Condition ORMWhere::LessThan
       Equivalent for \a <
       \var ORMWhere::Condition ORMWhere::LessOrEquals
       Equivalent for \a <=
       \var ORMWhere::Condition ORMWhere::GreaterThan
       Equivalent for \a >
       \var ORMWhere::Condition ORMWhere::GreaterOrEquals
       Equivalent for \a >=
       \var ORMWhere::Condition ORMWhere::StartsWith
       Find values, which start with \a param
       \var ORMWhere::Condition ORMWhere::EndsWith
       Find values, which end with \a param
       \var ORMWhere::Condition ORMWhere::Contains
       Find values, which contain \a param
       \var ORMWhere::Condition ORMWhere::IsNull
       Find values, which is null
     */
    enum Condition
    {
        Equals,
        NotEquals,
        LessThan,
        LessOrEquals,
        GreaterThan,
        GreaterOrEquals,
        StartsWith,
        EndsWith,
        Contains,
        IsNull
    };
    /*!
       Construct empty ORMWhere object
     */
    ORMWhere();
    /*!
       Construct ORMWhere object with the given parameters
       \param fieldName field name from which get values for comparison with \a value
       \param con condition
       \param value value
     */
    ORMWhere(QString fieldName, Condition con, QVariant value);
    /*!
       Assigns the b object to this object.
     */
    void operator= (ORMWhere b);
    /*!
       Logical AND for ORMWhere object.
       \return new ORMWhere object, which contain \a WHERE expression
     */
    ORMWhere operator&& (ORMWhere b);
    /*!
       Logical OR for ORMWhere object.
       \return new ORMWhere object, which contain \a WHERE expression
     */
    ORMWhere operator|| (ORMWhere b);
    /*!
       Return current \a WHERE expression
     */
    QString getWhereCondition();
private:
    QString m_whereCondition;
};

#endif // ORMWHERE_H