/* Copyright (C) 2007 Pierre Marchand
 *
 * Copyright (C) 2018 Marco Scarpetta
 *
 * This file is part of PDF Mix Tool.
 *
 * PDF Mix Tool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PDF Mix Tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PDF Mix Tool. If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <istream>
#include <ostream>
#include <iostream>
#include <cstdlib>
#include <cerrno>

#include "podofo_pdftranslator.h"

using std::ostringstream;
using std::map;
using std::vector;
using std::string;
using std::ifstream;
using std::istream;
using std::ostream;
using std::endl;
using std::runtime_error;

PdfTranslator::PdfTranslator( )
{
    std::cerr << "PdfTranslator::PdfTranslator" <<std::endl;
    sourceDoc = NULL;
    targetDoc = NULL;
    duplicate = 0;
    extraSpace = 0;
    scaleFactor = 1.0;
    pcount = 0;
    sourceWidth = 0.0;
    sourceHeight = 0.0;
    destWidth = 0.0;
    destHeight = 0.0;
}

void PdfTranslator::setSource(PdfMemDocument *source)
{
    sourceDoc = source;
    pcount = sourceDoc->GetPageCount();

    // only here to avoid possible segfault, but PDF without page is not conform IIRC
    if (pcount > 0)
    {
        PoDoFo::PdfRect rect (sourceDoc->GetPage(0)->GetMediaBox());
        // keep in mind it’s just a hint since PDF can have different page
        // sizes in a same doc
        sourceWidth =  rect.GetWidth() - rect.GetLeft();
        sourceHeight =  rect.GetHeight() - rect.GetBottom() ;
    }

    // Create target document
    targetDoc = new PdfMemDocument;

    for (int i = 0; i < pcount ; ++i)
    {
        PdfPage * page = sourceDoc->GetPage(i);
        PdfMemoryOutputStream outMemStream(1);

        PdfXObject *xobj = new PdfXObject (page->GetMediaBox(), targetDoc);
        if (page->GetContents()->HasStream())
        {
            page->GetContents()->GetStream()->GetFilteredCopy(&outMemStream);
        }
        else if (page->GetContents()->IsArray())
        {
            PdfArray carray(page->GetContents()->GetArray());
            for ( unsigned int ci = 0; ci < carray.GetSize(); ++ci )
            {
                if ( carray[ci].HasStream() )
                {
                    carray[ci].GetStream()->GetFilteredCopy ( &outMemStream );
                }
                else if ( carray[ci].IsReference() )
                {
                    PdfObject *co = sourceDoc->GetObjects().GetObject(
                                carray[ci].GetReference());

                    while ( co != NULL )
                    {
                        if ( co->IsReference() )
                        {
                            co = sourceDoc->GetObjects().GetObject ( co->GetReference() );
                        }
                        else if ( co->HasStream() )
                        {
                            co->GetStream()->GetFilteredCopy ( &outMemStream );
                            break;
                        }
                    }

                }

            }
        }

        /// Its time to manage other keys of the page dictionary.
        std::vector<std::string> pageKeys;
        std::vector<std::string>::const_iterator itKey;
        pageKeys.push_back ( "Group" );
        for ( itKey = pageKeys.begin(); itKey != pageKeys.end(); ++itKey )
        {
            PoDoFo::PdfName keyname ( *itKey );
            if ( page->GetObject()->GetDictionary().HasKey ( keyname ) )
            {
                xobj->GetObject()->GetDictionary().AddKey(
                            keyname,
                            migrateResource(
                                page->GetObject()->GetDictionary().GetKey(keyname))
                            );
            }
        }

        outMemStream.Close();

        PdfMemoryInputStream inStream ( outMemStream.TakeBuffer(),outMemStream.GetLength() );
        xobj->GetContents()->GetStream()->Set ( &inStream );

        resources[i+1] = getInheritedResources ( page );
        xobjects[i+1] = xobj;
        cropRect[i+1] = page->GetCropBox();
        bleedRect[i+1] = page->GetBleedBox();
        trimRect[i+1] = page->GetTrimBox();
        artRect[i+1] = page->GetArtBox();

    }


    targetDoc->SetPdfVersion ( sourceDoc->GetPdfVersion() );

    PdfInfo *sInfo ( sourceDoc->GetInfo() );
    PdfInfo *tInfo ( targetDoc->GetInfo() );

    if ( sInfo->GetAuthor() != PdfString::StringNull )
        tInfo->SetAuthor ( sInfo->GetAuthor() );
    if ( sInfo->GetCreator() != PdfString::StringNull )
        tInfo->SetCreator ( sInfo->GetCreator() );
    if ( sInfo->GetSubject() != PdfString::StringNull )
        tInfo->SetSubject ( sInfo->GetSubject() );
    if ( sInfo->GetTitle() != PdfString::StringNull )
        tInfo->SetTitle ( sInfo->GetTitle() );
    if ( sInfo->GetKeywords() != PdfString::StringNull )
        tInfo->SetKeywords ( sInfo->GetKeywords() );

    if ( sInfo->GetTrapped() != PdfName::KeyNull )
        tInfo->SetTrapped ( sInfo->GetTrapped() );


    // 	PdfObject *scat( sourceDoc->GetCatalog() );
    // 	PdfObject *tcat( targetDoc->GetCatalog() );
    // 	TKeyMap catmap = scat->GetDictionary().GetKeys();
    // 	for ( TCIKeyMap itc = catmap.begin(); itc != catmap.end(); ++itc )
    // 	{
    // 		if(tcat->GetDictionary().GetKey(itc->first) == 0)
    // 		{
    // 			PdfObject *o = itc->second;
    // 			tcat->GetDictionary().AddKey (itc->first , migrateResource( o ) );
    // 		}
    // 	}

    // 	delete sourceDoc;
}

PdfObject* PdfTranslator::migrateResource ( PdfObject * obj )
{
    // 			std::cerr<<"PdfTranslator::migrateResource"<<std::endl;
    PdfObject *ret ( 0 );

    if ( obj->IsDictionary() )
    {
        ret = targetDoc->GetObjects().CreateObject ( *obj );

        TKeyMap resmap = obj->GetDictionary().GetKeys();
        for ( TCIKeyMap itres = resmap.begin(); itres != resmap.end(); ++itres )
        {
            PdfObject *o = itres->second;
            ret->GetDictionary().AddKey ( itres->first , migrateResource ( o ) );
        }

        if ( obj->HasStream() )
        {
            * ( ret->GetStream() ) = * ( obj->GetStream() );
        }
    }
    else if ( obj->IsArray() )
    {
        PdfArray carray ( obj->GetArray() );
        PdfArray narray;
        for ( unsigned int ci = 0; ci < carray.GetSize(); ++ci )
        {
            PdfObject *co ( migrateResource ( &carray[ci] ) );
            narray.push_back ( *co );
        }
        ret = targetDoc->GetObjects().CreateObject ( narray );
    }
    else if ( obj->IsReference() )
    {
        if ( migrateMap.find ( obj->GetReference().ToString() ) != migrateMap.end() )
        {
            return migrateMap[obj->GetReference().ToString() ];
        }

        PdfObject *o(
                    migrateResource(
                        sourceDoc->GetObjects().GetObject (obj->GetReference()))
                    );

        ret  = new PdfObject ( o->Reference() ) ;

    }
    else
    {
        ret = new PdfObject ( *obj );//targetDoc->GetObjects().CreateObject(*obj);
    }


    migrateMap.insert (
                std::pair<std::string, PdfObject*> ( obj->Reference().ToString(), ret )
                );


    return ret;

}

PdfObject* PdfTranslator::getInheritedResources ( PdfPage* page )
{
    // 			std::cerr<<"PdfTranslator::getInheritedResources"<<std::endl;
    PdfObject *res ( 0 ); // = new PdfObject;
    PdfObject *rparent = page->GetObject();
    while ( rparent && rparent->IsDictionary() )
    {
        PdfObject *curRes = rparent->GetDictionary().GetKey ( PdfName ( "Resources" ) );
        if ( curRes )
        {
            res = migrateResource ( curRes );
        }
        rparent = rparent->GetIndirectKey ( "Parent" );
    }
    return res;

}

PdfMemDocument *PdfTranslator::impose(const NupSettings &nup_settings)
{
    // 			std::cerr<<"PdfTranslator::impose"<<std::endl;
    if ( !targetDoc )
        throw std::invalid_argument ( "impose() called with empty target" );

    int plate_page_count = nup_settings.rows * nup_settings.columns;

    double cosR = cos(nup_settings.rotation  *  3.14159 / 180.0);
    double sinR = sin(nup_settings.rotation  *  3.14159 / 180.0);

    double max_width = (nup_settings.page_width -
                        nup_settings.margin_left - nup_settings.margin_right -
                        nup_settings.spacing * (nup_settings.rows - 1)) / nup_settings.rows;

    double max_height = (nup_settings.page_height -
                         nup_settings.margin_top - nup_settings.margin_bottom -
                         nup_settings.spacing * (nup_settings.columns - 1)) / nup_settings.columns;

    scaleFactor = std::min(max_width/nup_settings.page_width, max_height/nup_settings.page_height);

    double page_width = sourceWidth / cm * scaleFactor;
    double page_height = sourceHeight / cm  * scaleFactor;

    double delta_x = 0;
    if (nup_settings.h_alignment == HAlignment::Center)
        delta_x = (max_width - page_width) / 2;
    else if (nup_settings.h_alignment == HAlignment::Right)
        delta_x = max_width - page_width;

    double delta_y = 0;
    if (nup_settings.v_alignment == VAlignment::Center)
        delta_y = (max_height - page_height) / 2;
    else if (nup_settings.v_alignment == VAlignment::Top)
        delta_y = max_height - page_height;

    //			PdfObject trimbox;
    //			PdfRect trim ( 0, 0, destWidth, destHeight );
    //			trim.ToVariant ( trimbox );
    std::map<int, PdfRect>* bbIndex = NULL;
    if(boundingBox.size() > 0)
    {
        if(boundingBox.find("crop") != std::string::npos)
        {
            bbIndex = &cropRect;
        }
        else if(boundingBox.find("bleed") != std::string::npos)
        {
            bbIndex = &bleedRect;
        }
        else if(boundingBox.find("trim") != std::string::npos)
        {
            bbIndex = &trimRect;
        }
        else if(boundingBox.find("art") != std::string::npos)
        {
            bbIndex = &artRect;
        }
    }

    int current_page = 1;

    while (current_page <= pcount)
    {
        PdfPage *newpage = targetDoc->CreatePage(
                    PdfRect (0.0, 0.0, nup_settings.page_width * cm, nup_settings.page_height * cm)
                    );

        // 		newpage->GetObject()->GetDictionary().AddKey ( PdfName ( "TrimBox" ), trimbox );

        PdfDictionary xdict;

        ostringstream buffer;
        // Scale
        buffer << std::fixed << scaleFactor << " 0 0 " << scaleFactor << " 0 0 cm\n";

        for (unsigned int i = 0; i < plate_page_count; i++)
        {
            // 					std::cerr<<curRecord.sourcePage<< " " << curRecord.destPage<<std::endl;
            if(current_page <= pcount)
            {
                double tx = (
                            nup_settings.margin_left +
                            (max_width + nup_settings.spacing) * (i % nup_settings.rows) +
                            delta_x * ((i % nup_settings.rows) + 1)
                            ) * cm / scaleFactor;
                double ty = (
                            nup_settings.page_height - nup_settings.margin_top - max_height -
                            (max_height + nup_settings.spacing) * (i / nup_settings.rows) +
                            delta_y * ((i / nup_settings.rows) + 1)
                            ) * cm / scaleFactor;

                /*(curRecord.duplicateOf > 0) ? curRecord.duplicateOf : */
                int resourceIndex(current_page);
                PdfXObject *xo = xobjects[resourceIndex];
                if(NULL != bbIndex)
                {
                    PdfObject bb;
                    // DominikS: Fix compilation using Visual Studio on Windows
                    // bbIndex->at(resourceIndex).ToVariant( bb );
                    ((*bbIndex)[resourceIndex]).ToVariant( bb );
                    xo->GetObject()->GetDictionary().AddKey ( PdfName ( "BBox" ), bb );
                }
                ostringstream op;
                op << "OriginalPage" << resourceIndex;
                xdict.AddKey ( PdfName ( op.str() ) , xo->GetObjectReference() );

                if ( resources[resourceIndex] )
                {
                    if ( resources[resourceIndex]->IsDictionary() )
                    {
                        TKeyMap resmap = resources[resourceIndex]->GetDictionary().GetKeys();
                        TCIKeyMap itres;
                        for ( itres = resmap.begin(); itres != resmap.end(); ++itres )
                        {
                            xo->GetResources()->GetDictionary().AddKey(
                                        itres->first, itres->second
                                        );
                        }
                    }
                    else if ( resources[resourceIndex]->IsReference() )
                    {
                        xo->GetObject()->GetDictionary().AddKey(
                                    PdfName ( "Resources" ), resources[resourceIndex]
                                    );
                    }
                    else
                        std::cerr << "ERROR Unknown type resource " <<
                                     resources[resourceIndex]->GetDataTypeString() <<
                                     std::endl;

                }
                // Very primitive but it makes it easy to track down imposition
                // plan into content stream.
                buffer << "q\n";
                buffer << std::fixed <<
                          cosR << " " << sinR << " " <<
                          -sinR << " " << cosR << " " <<
                          tx << " " <<  ty << " cm\n";
                buffer << "/OriginalPage" << resourceIndex << " Do\n";
                buffer << "Q\n";
            }

            ++current_page;
        }
        if (!newpage)
            PODOFO_RAISE_ERROR (ePdfError_ValueOutOfRange);
        string bufStr = buffer.str();
        newpage->GetContentsForAppending()->GetStream()->Set ( bufStr.data(), bufStr.size() );
        newpage->GetResources()->GetDictionary().AddKey ( PdfName ( "XObject" ), xdict );
    }

    return targetDoc;

}
