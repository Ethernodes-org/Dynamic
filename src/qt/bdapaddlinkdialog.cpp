// Copyright (c) 2016-2019 Duality Blockchain Solutions Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bdapaddlinkdialog.h"
#include "ui_bdapaddlinkdialog.h"
#include "bdaplinkdetaildialog.h"
#include "bdappage.h"

#include "guiutil.h"
#include "rpcregister.h"
#include "rpcserver.h"
#include "rpcclient.h"
#include "util.h"

#include <stdio.h>
#include <boost/algorithm/string.hpp>


BdapAddLinkDialog::BdapAddLinkDialog(QWidget *parent) : QDialog(parent),
                                                        ui(new Ui::BdapAddLinkDialog)
{
   
    ui->setupUi(this);

    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(goCancel()));
    connect(ui->pushButtonAddLink, SIGNAL(clicked()), this, SLOT(addLink()));


}

BdapAddLinkDialog::~BdapAddLinkDialog()
{
    delete ui;
}


void BdapAddLinkDialog::addLink()
{
    std::string requestor = "";
    std::string recipient = "";
    std::string linkMessage = "";
    std::string registrationDays = "";
    std::string outputmessage = "";

    JSONRPCRequest jreq;
    std::vector<std::string> params;

    requestor = ui->lineEditRequestor->text().toStdString();
    recipient = ui->lineEditRecipient->text().toStdString();
    linkMessage = ui->lineEditLinkMessage->text().toStdString();
    registrationDays = ui->lineEditRegistrationDays->text().toStdString();

    if ((requestor == "") || (recipient == "") || (linkMessage == "")) {
        QMessageBox::critical(this, "BDAP Add Link Error", QObject::tr("Requestor, Recipient and Link Message are required fields"));
        return;
    } //if requestor

    params.push_back("request");
    params.push_back(requestor);
    params.push_back(recipient);
    params.push_back(linkMessage);
    if (registrationDays.length() >> 0) params.push_back(registrationDays);
    jreq.params = RPCConvertValues("link", params);
    jreq.strMethod = "link";

    try {
        UniValue result = tableRPC.execute(jreq);

        outputmessage = result.getValues()[0].get_str();
        BdapLinkDetailDialog dlg(this,LinkActions::LINK_REQUEST,"","",result,true);
        dlg.setWindowTitle(QObject::tr("Successfully added link"));

        dlg.exec();
        goClose();
    } catch (const UniValue& objError) {
        //rpc_result = JSONRPCReplyObj(NullUniValue, objError, jreq.id);
        std::string message = find_value(objError, "message").get_str();
        outputmessage = ignoreErrorCode(message);
        QMessageBox::critical(this, "BDAP Add Link Error", QObject::tr(outputmessage.c_str()));
        return;
    } catch (const std::exception& e) {
        //rpc_result = JSONRPCReplyObj(NullUniValue,
        //JSONRPCError(RPC_PARSE_ERROR, e.what()), jreq.id);
        outputmessage = e.what();
        QMessageBox::critical(this, "BDAP Add Link Error", QObject::tr(outputmessage.c_str()));
        return;
    }


} //addLink



void BdapAddLinkDialog::goCancel()
{
    QDialog::reject(); //cancelled
} //goCancel


void BdapAddLinkDialog::goClose()
{
    QDialog::accept(); //accepted
} //goClose

std::string BdapAddLinkDialog::ignoreErrorCode(const std::string input)
{
    //assuming error code is in the following format: ERROR CODE - ERROR MESSAGE
    
    std::vector<std::string> results;
    std::string returnvalue = input;

    boost::split(results, input, [](char c){return c == '-';});

    if (results.size() > 1) {
        returnvalue = results[1];
    }

    return returnvalue;


} //ignoreErrorCode














