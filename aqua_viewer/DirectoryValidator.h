#ifndef DIRECTORYVALIDATOR_H
#define DIRECTORYVALIDATOR_H

class DirectoryValidator : public wxValidator
{
public:
    DirectoryValidator(const DirectoryValidator& tocopy) { m_var=tocopy.m_var; }
    DirectoryValidator(wxString* var) { m_var=var; }

	// Actually validate contents
    virtual bool Validate(wxWindow* parent);
    virtual wxObject* Clone() const { return new DirectoryValidator(*this); }

    // Called to transfer data to the window
    virtual bool TransferToWindow();

    // Called to transfer data from the window
    virtual bool TransferFromWindow();

protected:
    wxString* m_var;
};

#endif
